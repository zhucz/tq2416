cmd_sysklogd/lib.a := rm -f sysklogd/lib.a; /usr/local/arm-2014.05/bin/arm-none-linux-gnueabi-ar  rcs sysklogd/lib.a sysklogd/klogd.o sysklogd/logread.o sysklogd/syslogd_and_logger.o
