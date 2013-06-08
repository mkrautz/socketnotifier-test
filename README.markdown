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
