#############################
Glossary
#############################

Some `common` word that recur into the documentation have a specific meaning. Below an explanation of the terms and concepts used.

.. glossary::

	Project
		A project in ``licensecc`` terminology usually corresponds to one of your applications where the licensing system need to be integrated. 
	 	For instance you want to release the executable ``Foo.exe`` you may want to create a project ``Foo`` in licensecc.
	 	A ``licensecc-project`` is the set of customizations needed to compile licensecc-library for your own application:
		(1) a private key used to issue licenses,
		(2) a public key (included at compile time in licensecc library) used to verify licenses,
		(3) a set of build parameters and customizations specific to your application.
		One source code branch of licensecc may "serve" multiple projects: every time you build you may specify for which project
		you need to compile and the build system will select the right include files (eg: `public_key.h`).

	Features
		Features let licensed application to selectively enable or disable functions in the application.


