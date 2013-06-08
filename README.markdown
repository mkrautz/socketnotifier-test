When I run it:

    main: about to write 'x'
    thr: read 'x'!
    thr: wrote 'a'
    thr: now sleeping...
    QSNTester: creating socket notifier...
    main: main loop iter 0 entry
    main: main loop iter 0 exit
    main: main loop iter 1 entry
    readyRead invoked!
    readyRead read 'a'
    main: main loop iter 1 exit
    main: main loop iter 2 entry
    readyRead invoked!

Why is readyRead invoked twice, when there was only one write to the FD?

When run with strace (only tracing poll() syscalls), I get this:

    $ strace -q -e trace=poll,select ./qsntest 
    main:  tsock=3,  mainsock=4
    main: about to write 'x'
    thr: read 'x'!
    thr: wrote 'a'
    thr: now sleeping...
    QSNTester: creating socket notifier...
    main: main loop iter 0 entry
    poll([{fd=5, events=POLLIN}, {fd=4, events=POLLIN}], 2, 0) = 2 ([{fd=5, revents=POLLIN}, {fd=4, revents=POLLIN}])
    main: main loop iter 0 exit
    main: main loop iter 1 entry
    poll([{fd=5, events=POLLIN}, {fd=4, events=POLLIN}], 2, 0) = 2 ([{fd=5, revents=POLLIN}, {fd=4, revents=POLLIN}])
    readyRead invoked!
    readyRead read 'a'
    main: main loop iter 1 exit
    main: main loop iter 2 entry
    poll([{fd=5, events=POLLIN}, {fd=4, events=POLLIN}], 2, 0) = 1 ([{fd=5, revents=POLLIN}])
    readyRead invoked!

Seems like the 0th main loop iteration finds that the socket is ready for reading, and queues
up the slot invocation to the next main loop iteration.  In the 1st iteration, it *again* finds
that the socket is ready for reading, and *after* that, invokes the readyRead() slot of the
QSocketNotifier (the one from the 0th iteration).

The 'hang' must then be because the queued-up slot invocation from the 1st iteration isn't invoked until
the 2nd iteration, where the FD isn't ready for reading anymore (the readyRead() that was executed
during the 1st iteration took care of it).
