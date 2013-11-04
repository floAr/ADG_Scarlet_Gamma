Code Conventions
===

1. Structure
---
* Header: CamelCase.hpp
* Source: CamelCase.cpp
* Header and Source of the same name always in the same directory
* directories give structure the project
* namespace conform with the directory structure
* Use pre-declarations instead of includes whenever possible

1. Documentation
---
* Doxygen comments in the Header at public attributes/methods:
	/// \brief			always write a brief describtion
	/// \details		if there is more to say
	/// \param [in]		if parameter is only read inside method
	/// \param [out]	if parameter is only written inside method
	/// \param [inout]	the parameter is accessed with read write access
	/// \return
* Member variables and constants: ///< at end of line if short
* *.cpp contains standard comments: //

1. Code
---
* Everything in english
* Functions: CamelCase
* Types: CamelCase

* Variables: smallCamelCase
	* Scope-prefix:
	  'm_' Member of a class (Example: m_myInt)
	  'g_' Global variable
	  '_' Function parameter
	* No type prefixes
* Constants: BIG_LETTERS