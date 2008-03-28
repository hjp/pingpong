#!/bin/sh
echo "\$(DESTDIR)\$(BINDIR)/%: %"
echo -e "\tcp \$^ \$@"
echo "\$(DESTDIR)\$(MAN1DIR)/%: %"
echo -e "\tcp \$^ \$@"
