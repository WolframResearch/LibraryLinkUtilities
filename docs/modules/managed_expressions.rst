======================
Managed expressions
======================

One of the features offered by LibraryLink is Managed Library Expressions (MLEs). The idea is to create C/C++ objects
that will be automatically deleted when they are no longer referenced in the Wolfram Language code. More information can
be found in the official LibraryLink `documentation <https://reference.wolfram.com/language/LibraryLink/tutorial/InteractionWithWolframLanguage.html#353220453>`_.

This allows for an object-oriented programming paradigm and it is the recommended way of referencing
C++ objects from Wolfram Language. The two most notable alternatives are:

* recreating C++ objects every time a library function is called

* maintaining some sort of global cache with referenced objects, where each object is added on first use and manually deleted at some point.

LLU uses methods similar to the second alternative to facilitate using MLEs and decrease the amount of boilerplate
code needed from developers. Namely, for each class that you register to be used as MLE, LLU will maintain a map, which
associates managed C++ objects with IDs assigned to them by Wolfram Language.

Register a class as Managed Expression
=========================================

Imagine you have a class `A` whose objects you want to manage from Wolfram Language. The class needs to be defined:

.. code-block:: cpp
   :dedent: 1

	struct A {
	    A(int n) : myNumber{n} {}
	    int getMyNumber() const { return myNumber; }
	private:
	    int myNumber;
	};

Then you must create the corresponding Store and specialize a callback function for LibraryLink (this is a technicality
that just needs to be done):

.. code-block:: cpp
   :dedent: 1

	LLU::ManagedExpressionStore<ClassName> AStore;  //usually <class name>Store is a good name

	//specialize manageInstanceCallback, this should just call manageInstance function from your Store
	template<>
	void LLU::manageInstanceCallback<A>(WolframLibraryData, mbool mode, mint id) {
	    AStore.manageInstance(mode, id);
	}


The last step is to actually register and unregister your type when library gets loaded or unloaded, respectively.

.. code-block:: cpp
   :dedent: 1

	EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
		LLU::LibraryData::setLibraryData(libData);
		AStore.registerType("A");   // the string you pass is the name of a symbol that will be used in Wolfram Language for managing
		return 0;                   // objects of your class, it is a good convention to just use the class name
	}

	EXTERN_C DLLEXPORT void WolframLibrary_uninitialize(WolframLibraryData libData) {
		AStore.unregisterType(libData);
	}

The good news is that you can omit the middle step thanks to a special macro:

.. code-block:: cpp

   DEFINE_MANAGED_STORE_AND_SPECIALIZATION(A)

This will define the Store variable and the template specialization for you but you still need to register your class in
`WolframLibrary_initialize` and unregister in `WolframLibrary_uninitialize`. The macro must be called in a scope where structure `A` is defined.

One of the biggest limitations of MLEs in LibraryLink is that you cannot pass arguments for construction of managed expressions.
This is addressed in LLU by letting the developer define a library function that LLU will call from Wolfram Language
when new instance of a managed expression is created. In other words, define a wrapper for constructor of your class.
Typically, it will look like this:

.. code-block:: cpp
   :dedent: 1

	EXTERN_C DLLEXPORT int OpenManagedA(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
		auto err = LLU::ErrorCode::NoError;
		try {
			LLU::MArgumentManager mngr(libData, Argc, Args, Res);
			auto id = mngr.getInteger<mint>(0); // id of the object to be created
			auto arg1 = mngr.getXXXX(1);
			auto arg2 = mngr.getYYYY(2);
			... // read the rest of parameters for constructor of your managed class
			AStore.createInstance(id, arg1, arg2, ...);
		} catch (const LLU::LibraryLinkError& e) {
			err = e.which();
		}
		return err;
	}


It is simpler to register an MLE in the Wolfram Language. You only need to load your constructor wrapper:

.. code-block:: mathematica

   LLU`Constructor[A] = SafeLibraryFunction["OpenManagedA", {LLU`Managed[A], Arg1Type (*, ...*)}, "Void"];



Using Managed Expressions
=========================================

After the registration is done, using MLEs is very simple. In C++ code, MLEs can be treated as another MArgument type,
for example let's define a wrapper over `A::getMyNumber()`:

.. code-block:: cpp
   :dedent: 1

	LIBRARY_LINK_FUNCTION(GetMyNumber) {
		auto err = LLU::ErrorCode::NoError;
		try {
			LLU::MArgumentManager mngr(Argc, Args, Res);
			const A& myA = mngr.getManagedExpression(0, AStore);
			mngr.set(myA.getmyNumber());
		} catch (const LLU::LibraryLinkError &e) {
			err = e.which();
		}
		return err;
	}

In the Wolfram Language, wrappers over member functions can be conveniently loaded:

.. code-block:: wolfram-language
   :dedent: 1

	LLU`LoadMemberFunction[A][
	    getMyNumber (* fresh symbol for the member function *)
	    "GetMyNumber", (* function name in the library *)
	    {} (* argument list *),
	    Integer (* result type *)
	];


The only thing left now is to create an MLE instance and call the member function on it:

.. code-block:: wolfram-language

   myA = LLU`NewManagedExpression[A][17];

   myA @ getMyNumber[]
   (* = 17 *)


API Reference
=========================================

.. doxygenfunction:: LLU::manageInstanceCallback

.. doxygenclass:: LLU::ManagedExpressionStore
   :members:
