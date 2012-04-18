ACLOCAL_AMFLAGS = -I m4
SUBDIRS = src include
EXTRA_DIST = examples

if HAVE_DOXYGEN
doxygen = @DOXYGEN@

all-local:
	$(doxygen) doxygen.cfg

clean-local:
	-rm -rf $(doxygen_output_directory)

install-data-local:
	$(INSTALL) -d $(docdir)
	for dir in $(doxygen_output_directory)/* ; do \
		cp -R $${dir} $(docdir) ; \
	done

uninstall-local:
	-rm -rf $(docdir)

endif
