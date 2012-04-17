ACLOCAL_AMFLAGS = -I m4
SUBDIRS = src include
EXTRA_DIST = examples

if HAVE_DOXYGEN
doxygen = @DOXYGEN@
doxygen_output_directory = @doxygen_output_directory@

doc:
	$(doxygen) doxygen.cfg

clean-doc:
	-rm -rf $(doxygen_output_directory)

install-doc: doc
	$(INSTALL) -d $(docdir)
	for dir in $(doxygen_output_directory)/* ; do \
		cp -R $${dir} $(docdir) ; \
	done

uninstall-doc:
	-rm -rf $(docdir)

all: doc

clean: clean-doc

install: install-doc

uninstall: uninstall-doc

endif
