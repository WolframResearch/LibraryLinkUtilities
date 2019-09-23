===================
MathLink support
===================

_LibraryLink_ allows you to pass LinkObject as argument, which may then be utilized to exchange data between your library and the Kernel using MathLink.
The original MathLink API is in old C style with error codes, macros, manual memory management, etc. Therefore, __LLU__ provides a wrapper for the LinkObject called `MLStream`.

`MLStream` is actually a class template parameterized by the default encoding to be used for strings, but for the sake of clarity, the template parameter is skipped in the remainder of this README.


Main features
====================

Convenient syntax
-----------------------

In this extension to __LLU__ MathLink is interpreted as an i/o stream, so operators << and >> are utilized to make the syntax cleaner and more concise.
This means that the framework frees the developer from the responsibility to choose proper MathLink function for the data they intend to read or write.

Error checking
-----------------------

Each call to MathLink has its return status checked. In case of failure an exception is thrown. Such exceptions carry some debug info to help locate the problem.
Sample debug info looks like this::

	Error code reported by MathLink: 48
	"Unable to convert from given character encoding to MathLink encoding"
	Additional debug info: MLPutUTF8String


Memory cleanup
-----------------------

You're no longer required to call `MLRelease*` on the data received from MathLink. The framework does it for you.

Automated handling of common data types
--------------------------------------------------

Some sophisticated types can be sent to Mathematica directly via `MLStream` class. For example nested maps:

.. code-block:: cpp

	std::map<std::string, std::map<int, std::vector<double>>> myNestedMap


Just write `ms << myNestedMap` and you will get a nested Association on the other side. It works in the other direction as well.
Obviously, for the above to work, key and value type in the map must be supported by MathLink.

If you have any particular type that you think should be directly supported by `MLStream`, please let me know.

User-defined classes
----------------------------------------

Suppose you have a structure

.. code-block:: cpp

	struct Color {
	    double red;
	    double green;
	    double blue;
	};


It is enough to overload `operator<<` like this:

.. code-block:: cpp
   :linenos:
   :dedent: 1

	MLStream& operator<<(MLStream& ms, const Color& c) {
	    return ms << ML::Function("RGBColor", 3) << c.red << c.green << c.blue;
	}


And now you're able to send objects of class `Color` directly via `MLStream`.

Expressions of unknown length
-----------------------------------------------

Whenever you send an expression via MathLink you have to first specify the head and the number of arguments. It proved multiple times that this design is not very flexible,
for example when you are reading from a file and cannot easily tell how much data is left.

As a workaround, one can create a temporary loopback link, accumulate all the arguments there (without the head),
count the arguments and then send everything to the "main" link as usual.

The same strategy has been incorporated into `MLStream` so that developers do not have to implement it manually any longer. Now you can send a `List` like this:

.. code-block:: cpp
   :linenos:
   :dedent: 1

	MLStream ms(mlp);

	ms << ML::BeginExpr("List");
	while (dataFromFile != EOF) {
		// process data from file and send to MLStream
	}
	ms << ML::EndExpr();


.. warning::

	This feature should only be used if necessary since it requires a temporary link and makes extra copies
	of data. Simple benchmarks showed a ~2x slowdown compared to the usual `MLPutFunction`.


Example
=============

Let's compare the same piece of code written in plain _LibraryLink_ with one written with _LLU_ and `MLStream`.
Take a look at the code snippet taken from one of the Import/Export paclets:

.. code-block:: cpp
   :dedent: 1

	if (!MLNewPacket(mlp)) {
	    wsErr = -1;
	    goto cleanup;
	}
	if (!MLPutFunction(mlp, "List", nframes)) {
	    wsErr = -1;
	    goto cleanup;
	}
	for (auto& f : extractedFrames) {
	    if (!MLPutFunction(mlp, "List", 7)) {
	        wsErr = -1;
	        goto cleanup;
	    }
	    if (!MLPutFunction(mlp, "Rule", 2)) {
	        wsErr = -1;
	        goto cleanup;
	    }
	    if (!MLPutString(mlp, "ImageSize")) {
	        wsErr = -1;
	        goto cleanup;
	    }
	    if (!MLPutFunction(mlp, "List", 2)) {
	        wsErr = -1;
	        goto cleanup;
	    }
	    if (!MLPutInteger64(mlp, f->width)) {
	        wsErr = -1;
	        goto cleanup;
	    }
	    if (!MLPutInteger64(mlp, f->height)) {
	        wsErr = -1;
	        goto cleanup;
	    }
	    // ...
	    if (!MLPutFunction(mlp, "Rule", 2)) {
	        wsErr = -1;
	        goto cleanup;
	    }
	    if (!MLPutString(mlp, "ImageOffset")) {
	        wsErr = -1;
	        goto cleanup;
	    }
	    if (!MLPutFunction(mlp, "List", 2)) {
	        wsErr = -1;
	        goto cleanup;
	    }
	    if (!MLPutInteger64(mlp, f->left)) {
	        wsErr = -1;
	        goto cleanup;
	    }
	    if (!MLPutInteger64(mlp, f->top)) {
	        wsErr = -1;
	        goto cleanup;
	    }
	    // ...
	    if (!MLPutFunction(mlp, "Rule", 2)) {
	        wsErr = -1;
	        goto cleanup;
	    }
	    if (!MLPutString(mlp, "UserInputFlag")) {
	        wsErr = -1;
	        goto cleanup;
	    }
	    if (!MLPutSymbol(mlp, f->userInputFlag == true ? "True" : "False")) {
	        wsErr = -1;
	        goto cleanup;
	    }
	}
	if (!MLEndPacket(mlp)) {
		/* unable to send the end-of-packet sequence to mlp */
	}
	if (!MLFlush(mlp)){
		/* unable to flush any buffered output data in mlp */
	}

and now the same code using `MLStream`:

.. code-block:: cpp
   :dedent: 1

	MLStream ms(mlp);

	ms << ML::NewPacket;
	ms << ML::List(nframes);

	for (auto& f : extractedFrames) {
	    ms << ML::List(7)
	        << ML::Rule
	            << "ImageSize"
	            << ML::List(2) << f->width << f->height
	        // ...
	        << ML::Rule
	            << "ImageOffset"
	            << ML::List(2) << f->left << f->top
	        // ...
	        << ML::Rule
	            << "UserInputFlag"
	            << f->userInputFlag
	}

	ms << ML::EndPacket << ML::Flush;


API reference
================

.. doxygenclass:: LLU::MLStream
   :members: