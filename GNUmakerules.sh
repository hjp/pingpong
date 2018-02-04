#!/bin/sh
echo "\$(DESTDIR)\$(BINDIR)/%: %"
echo "\tcp \$^ \$@"
echo "\$(DESTDIR)\$(MAN1DIR)/%: %"
echo "\tcp \$^ \$@"
