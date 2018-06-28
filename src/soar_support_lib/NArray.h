/**
* \file NArray.h
* \brief Declaration and inline implementation of class NArray
*
* (C) Karsten Isakovic, Berlin 2016 ( Karsten.Isakovic@web.de )
*/

#ifndef NARRAY_H
#define NARRAY_H

#include <memory.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef  UNIT_TEST
#define assert(expression)	{ if ( !(expression) ) throw 42; }
#else
#include <assert.h>
#endif

/**
 * \ingroup SoarSupport
 * \ingroup SoarLib
 * \brief Template class for optimized N dimensional arrays.
 *
 * <DIV class="groupHeader">Features</DIV>
 *  - Fast linarized n dimensional arrays of varius data types
 *  - Parameter checking if compiled with _DEBUG 
 *  - Special constructor for convenient 1D array initialization
 *  - Support for [] operator with 1D arrays
 *  - Serialization and deserialization
 *  - Support for up to 10-dimensional arrays, easy to extend
 *  - Copy and assignment operators
 *  - Unit tested with 100% coverage with various test cases
 *  - Commented with doxygen 
 *
 * <DIV class="groupHeader">Usage</DIV>
 * \code
 *  // Usage example for 2D array
 *  NArray<int> arr0;           // Declare array with <int> type
 *  arr0.Init( 7, 3);           // Setup 2D dimensionality, all initilized to 0
 *  arr0.Fill( 42 );            // Initialize all members to 42
 *
 *  arr0(2,1) = 10;             // Set [2][1] to the value 10
 *  int b = arr0(5,1);          // Usage of [5][1]
 *
 *
 *  // Initialization of 3D array with 5 x 10 x 7 floats
 *  NArray<float> arr1;         // Declaration
 *  arr1.Init( 5,10, 7);        // Setup 3D dimensionality
 *
 *  // Initialization of 4D array with 4 x 2 x 7 x 9 booleans
 *  NArray<bool> arr2(4,2,7,9);// Declaration and setup in one line
 *
 *
 *  // Convenient 1D array initialization 
 *  int myVal[] = { 1,2,3,4,5 };// Static array data source
 *  NArray<int> arr3( myVal );  // Declaration, 1D setup and and data copy
 * 
 *  NArray<int> othArr;         // Declaration
 *  othArr = myVal;             // Setup and data copy of static array
 *
 *  // Convenient 1D array access via [] operator
 *  int a = othArr[2];          // Read from [2]
 *  othArr[3] = 7;              // Write  to [3]
 *
 *
 *  // Copy operations 
 *  NArray<int> arr4( arr3 );   // Declaration, setup and data copy
 *
 *  NArray<float> arr5;         // Declaration
 *  arr5 = arr4;                // Setup and data copy of NArray
 * \endcode
 *
 * <DIV class="groupHeader">Adaption to higher dimensionality</DIV>
 *   To add more dimensions, just add more entries to the NArrayDims type, 
 *   more parameters to Init() and add the access operators. You don't need 
 *   to touch the rest of the template, since everything should adapt to the 
 *   dimensionality.
 *
 * <DIV class="groupHeader">Versions</DIV>
 * \code
 *   13.03.2016 Added new constructor with array initialization
 *   03.03.2016 Updated doxygen documentation
 *   20.01.2016 Added more checks for _DEBUG
 *   16.01.2016 Added operator[] for 1D array access
 *   13.01.2016 Added convenient 1D array initialization
 *              Copy and assignment operators
 *   12.01.2016 Assembled into a single template class
 *   15.12.2015 All Array classes derive from ArrayBase
 *              Changed indices to unsigned int
 *   12.04.2015 Initial version
 * \endcode
 *
 * <DIV class="groupHeader">Copyright</DIV>
 * (C) Karsten Isakovic, Berlin 2016 ( Karsten.Isakovic@web.de )
 * <BR/>
 * <HR />
 *
 * <H2 class="groupheader">Include and Inheritance</H2>
 */
template<class T> class NArray {

private:
	
	/// \cond
	/**
	 * Dimensionality array definition. 
	 * Union allows acces to either members or as array.
	 */
	union NArrayDims {
		struct  {
			unsigned int dim0;
			unsigned int dim1;
			unsigned int dim2;
			unsigned int dim3;
			unsigned int dim4;
			unsigned int dim5;
			unsigned int dim6;
			unsigned int dim7;
			unsigned int dim8;
			unsigned int dim9;
		} sizes;
		unsigned int arr[10];
	} ;

	/// \endcond

	///

	NArrayDims   _dims;		///< Storage for the dimensions. 
	NArrayDims   _offs;		///< Storage for the offsets per dimension
	unsigned int _n;		///< Number of dimensions
	unsigned int _size;		///< Storage size for the data
	T *          _data;		///< The data of this array

	unsigned int _maxDims;	///< Maxxium dimensionality supported (calculated)
	
	/**
	 * \brief Re-Initializes the NArray to be empty. 
	 * Sets all member variables, frees _data if something had been allocated.
	 */
	void Reset() 
	{		
		if (_data!=0)
			delete [] _data;

		_maxDims = sizeof(NArrayDims) / sizeof(unsigned int);
		_data  = 0;
		_size  = 0;
		_n     = 0;
		memset(_dims.arr,0, sizeof(NArrayDims) );
		memset(_offs.arr,0, sizeof(NArrayDims) );
	}

	/**
	 * \brief Initializes the size, offset array and data storage based 
	 * on _n and _dims.
	 *
	 * Reset() must have been called before and after that
	 * at least _n and _dims.sizes.dim0 need to be set
	 */
	void InitSizeOffsData()
	{
		// Reset() must have been called before. And after that
		// at least _n and _dims.sizes.dim0 need to be set

		// Compute size
		_size = 1;
		for(unsigned int i=0;i<_n;i++) {
			_size *= _dims.arr[i];
		}
		
		// Compute offsets
		int off = 1;
		for(unsigned int i=0;i<_n;i++) {
			_offs.arr[i] = off;
			off *= _dims.arr[i];
		}

		_data = new T[_size];

		// Initialize the memory
		memset(_data,0, sizeof(T)*_size);
	}

	/**
	 * \brief Construct a copy of an NArray.
	 *
	 * The copy allocates own data memory and
	 * copies everything over.
	 *
	 * \param src The source NArray
	 */
	void CopyFrom(const NArray &src) 
	{	
		Reset();

		_n       = src._n;
		_maxDims = src._maxDims;
		_size    = src._size;
		memcpy(_dims.arr, src._dims.arr, sizeof(NArrayDims) );
		memcpy(_offs.arr, src._offs.arr, sizeof(NArrayDims) );

		_data = new T[_size];

		memcpy(_data, src._data, _size*sizeof(T) );
	}

public:
	
	/**
	 * \name Constructors and destructor
	 * @{ 
	 */

	//! The constructor. Initializes all members to be empty.
	NArray() : _data(0) {
		Reset();
	}
	
	//! The destructor. Frees all data memory
	~NArray() {
		if (_data!=0)
			delete [] _data;
	}

	/**
	 * \brief Special constructor for convenient 1D array initialization.
	 *
	 * This allows for the following construct
	 *
	 *   float myVal[] = { 5.0, 1.1, 7.2, 10.9, 8.3 };
	 *   NArray<float> myArr(myVal);
	 *
	 * The resulting myArray is initialized to 1D with 5 entries copied 
	 * from the static float array.
	 */ 
	template <typename TT, unsigned int NN>
		NArray(const TT (&arr) [NN]) : _data(0) {		
			Reset();

			_n    = 1;
			_dims.sizes.dim0 = NN;

			InitSizeOffsData();

			for (unsigned int i = 0; i < _dims.sizes.dim0; ++i) 
				_data[i] = arr[i]; 
	}


	/**
	 * \brief Copy constructor. 
	 * The copy operator allocates own data memory and copies everything over.
	 *
	 * \param src The source NArray
	 */
	NArray (const NArray &src) : _data(0) {	
		CopyFrom(src);
	}

	/**
	 * \brief Constructor with initialization.
	 * See Init() for a description of the parameters
	 */
	NArray(unsigned int d0, unsigned int d1=0, unsigned int d2=0, unsigned int d3=0, 
		unsigned int d4=0, unsigned int d5=0, unsigned int d6=0, unsigned int d7=0, 
		unsigned int d8=0, unsigned int d9=0) : _data(0) {
			Init(d0,d1,d2,d3,d4,d5,d6,d7,d8,d9);
	}

	///@} End of group started by \name


	/**
	 * \name Operators
	 * @{ 
	 */

	/**
	 * \brief Assignment operator. 
	 * The assignment operator allocates own data memory and copies everything over.
	 *
 	 * \param src The source NArray
	 * \return This instance (For  a=b=c; instructions)
	 */	
	NArray & operator=(const NArray &src) {

		// If This instance is already initialized, only allow copy of same dimensionality
		if (_n!=0) {
			// Verify dimensionality and dimensions of destination to be the same
			assert(_n == src._n);
			for(unsigned int i=0; i<_n; i++) {
				assert(_dims.arr[i] == src._dims.arr[i]);
			}
		}

		CopyFrom(src);
		return *this;
	}

	/**
	 * \brief Comparison operator. 
	 *
 	 * \param other The source NArray
	 * \return True if the same
	 */	
	bool operator==(const NArray &other) const {
		// Check dimensionality
		if (other._n != _n)
			return false;

		// Compare width of each dimension
		for(unsigned int i=0; i<_n; i++) {
			if (other._dims.arr[i] != _dims.arr[i] )
				return false;
		}
		
		// Shortcut for comparison of two empty NArrays
//		if (_n == 0)
//			return true;

		// Compare the data
		bool same = (memcmp(other._data,_data,_size*sizeof(T))==0);
		return same;
	}

	///@} End of group started by \name

	/**
	 * \name Configuration retrieval
	 * @{ 
	 */

	//! Retrieve the size of the data storage
    unsigned int     GetSize()	{ return _size; }

	//! Retrieve the dimensionality
	unsigned int     GetDims()	{ return _n; }

	/**
	 * \brief Retrieve each of the size of each dimension
	 * \param i  The dimension
	 * \return size of the requested dimension
	 */
	unsigned int     GetDim(unsigned i)	{ 
		assert(i>=0 && i<_n);
		return _dims.arr[i];
	}

	//! Retrieve the data storage
    T *GetData()	{ return _data; }

	///@} End of group started by \name

	

	/**
	 * \name Initialization
	 * @{ 
	 */

	/**
	 * \brief Initialization of NArray dimensions and dimensionality.
	 *
	 * Using default parameter values of 0, this member 
	 * function can be called with 1 to 10 parameters. 
	 * All non-supplied paramaters are automatically set to 
	 * zero.
	 * 
	 * If Init() is for instance called with 3 parameters a 
	 * 3 dimensional linear array layout is setup.
	 *
	 * \param d0 Size of 1. dimension 
	 * \param d1 Size of 2. dimension (optional)
	 * \param d2 Size of 3. dimension (optional)
	 * \param d3 Size of 4. dimension (optional)
	 * \param d4 Size of 5. dimension (optional)
	 * \param d5 Size of 6. dimension (optional)
	 * \param d6 Size of 7. dimension (optional)
	 * \param d7 Size of 8. dimension (optional)
	 * \param d8 Size of 9. dimension (optional)
	 * \param d9 Size of 10. dimension (optional)
	 */
	void Init(unsigned int d0, unsigned int d1=0, unsigned int d2=0, 
		unsigned int d3=0, unsigned int d4=0, unsigned int d5=0, 
		unsigned int d6=0, unsigned int d7=0, unsigned int d8=0, 
		unsigned int d9=0) {

		Reset();

		// Store  dimensions
		_dims.sizes.dim0 = d0;
		_dims.sizes.dim1 = d1;
		_dims.sizes.dim2 = d2;
		_dims.sizes.dim3 = d3;
		_dims.sizes.dim4 = d4;
		_dims.sizes.dim5 = d5;
		_dims.sizes.dim6 = d6;
		_dims.sizes.dim7 = d7;
		_dims.sizes.dim8 = d8;
		_dims.sizes.dim9 = d9;

		// Find dimensionality
		_n    = 10;
		for(unsigned int i=0;i<_maxDims;i++) {
			if (_dims.arr[i]==0) {
				_n = i;
				break;
			}
		}
		assert(_n!=0);	// At least one-dimensional, assert fails if d0=0

		// Check that there are no further params to init beyond he first 0
		for(unsigned int i=_n;i<_maxDims;i++) {
			assert( _dims.arr[i]==0);	
		}

		// Initialize size and offset structure, allocate data memory
		InitSizeOffsData();
	}
	
	/**
	 * \brief Fills the whole array with a value.
	 * This can be used for initialization.
	 *
	 * \param value  The value
	 */
	void Fill(T value) {
		for(unsigned int d=0; d<_size; d++)
			_data[d] = value;
	}

	///@} End of group started by \name

	
	/**
	 * \name Binary IO functions, can be called by ParamManager
	 * @{ 
	 */

	/**
	 * \brief Deserialize an NArray from a binary file
	 * \param file  Filepointer
	 */
	bool LoadBinary(FILE *file) {
		Reset();

		// Load dimensionailty and width of each dimensions
		if (fread(&_n, sizeof(unsigned int), 1, file) != 1)
			return false;

		if (_n>=_maxDims) {
			// Load failure
			// Probabbly wrong offset into file so that a higher number is
			// read for dimensionality instead of the correct one
			Reset();
			return false;
		}

		for(unsigned int i=0; i < _n; i++) {
			if (fread(&_dims.arr[i], sizeof(unsigned int), 1, file) != 1) {
				Reset();
				return false;
			}

			if (_dims.arr[i] >= 100000000) {
				// Load failure - diemnsion width extremly high.
				// Probabbly wrong offset into file so that a higher number is
				// read for width of dimension instead of the correct one
				Reset();
				return false;
			}
		}

		// Special case for loading empty NArray
		if (_n==0) {
			return true;
		}

		// Initialize size and offset structure, allocate data memory
		InitSizeOffsData();

		// Load the data
		return fread(_data, sizeof(T), _size, file)==_size;
	}
	

	/**
	 * \brief Serialize the NArray to a binary file
	 * \param file  Filepointer
	 */
	bool SaveBinary(FILE *file) {
		// Save dimensionality and width of each dimensions
		if (fwrite(&_n,  sizeof(unsigned int), 1, file) != 1)
			return false;

		for(unsigned int i=0; i < _n; i++) {
			if ( fwrite(&_dims.arr[i],  sizeof(unsigned int), 1, file) != 1)
				return false;
		}

		// Special case for saving empty NArray
		if (_n==0) {
			return true;
		}

		// Save the data
		return fwrite(_data,  sizeof(T), _size, file) == _size;
	}
	/**
	 * \brief Compare instance with serialization in file
	 * \param file  Filepointer
	 */
	bool CompareBinary(FILE *file) {
		NArray<T> tmpArr;

		// Load the other NArray into temporary memory
		if (!tmpArr.LoadBinary(file) )
			return false;

		return tmpArr==(*this);
	}

	///@} End of group started by \name


	/**
	 * \name Array access operators, one function for each supported dimensionality
	 * @{ 
	 */

	//! Special array access operator for 1 dimensional array
	T & operator[](unsigned int d0) {
		assert(_n==1);			// Check dimensionality
		assert(d0<_dims.sizes.dim0);

		return _data[d0];	
	}

	//! Array access operator for 1 dimensional array
	T & operator()(unsigned int d0) {
		assert(_n==1);			// Check dimensionality
		assert(d0<_dims.sizes.dim0);

		unsigned int  off =  d0;
		return _data[off];	
	}

	//! Array access operator for 2 dimensional array
	T & operator()(unsigned int d0, unsigned int d1) {
		assert(_n==2);			// Check dimensionality
		assert(d0<_dims.sizes.dim0);
		assert(d1<_dims.sizes.dim1);

		unsigned int  off =  d0 
				  + d1 * _offs.sizes.dim1;
		return _data[off];	
	}

	//! Array access operator for 3 dimensional array
	T & operator()(unsigned int d0, unsigned int d1, unsigned int d2) {
		assert(_n==3);			// Check dimensionality
		assert(d0<_dims.sizes.dim0);
		assert(d1<_dims.sizes.dim1);
		assert(d2<_dims.sizes.dim2);

		unsigned int  off =  d0 
				  + d1 * _offs.sizes.dim1 
				  + d2 * _offs.sizes.dim2;
		return _data[off];	
	}

	//! Array access operator for 4 dimensional array
	T & operator()(unsigned int d0, unsigned int d1, unsigned int d2, 
			unsigned int d3) {
		assert(_n==4);			// Check dimensionality
		assert(d0<_dims.sizes.dim0);
		assert(d1<_dims.sizes.dim1);
		assert(d2<_dims.sizes.dim2);
		assert(d3<_dims.sizes.dim3);

		unsigned int  off =  d0 
				  + d1 * _offs.sizes.dim1 
				  + d2 * _offs.sizes.dim2 
				  + d3 * _offs.sizes.dim3;

		return _data[off];	
	}

	//! Array access operator for 5 dimensional array
	T & operator()(unsigned int d0, unsigned int d1, unsigned int d2, 
			unsigned int d3, unsigned int d4) {
		assert(_n==5);			// Check dimensionality
		assert(d0<_dims.sizes.dim0);
		assert(d1<_dims.sizes.dim1);
		assert(d2<_dims.sizes.dim2);
		assert(d3<_dims.sizes.dim3);
		assert(d4<_dims.sizes.dim4);

		unsigned int  off =  d0 
				  + d1 * _offs.sizes.dim1 
				  + d2 * _offs.sizes.dim2 
				  + d3 * _offs.sizes.dim3 
				  + d4 * _offs.sizes.dim4;

		return _data[off];	
	}

	//! Array access operator for 6 dimensional array
	T & operator()(unsigned int d0, unsigned int d1, unsigned int d2, 
			unsigned int d3, unsigned int d4, unsigned int d5) {
		assert(_n==6);			// Check dimensionality
		assert(d0<_dims.sizes.dim0);
		assert(d1<_dims.sizes.dim1);
		assert(d2<_dims.sizes.dim2);
		assert(d3<_dims.sizes.dim3);
		assert(d4<_dims.sizes.dim4);
		assert(d5<_dims.sizes.dim5);

		unsigned int  off =  d0 
				  + d1 * _offs.sizes.dim1 
				  + d2 * _offs.sizes.dim2 
				  + d3 * _offs.sizes.dim3 
				  + d4 * _offs.sizes.dim4 
				  + d5 * _offs.sizes.dim5;

		return _data[off];	
	}

	//! Array access operator for 7 dimensional array
	T & operator()(unsigned int d0, unsigned int d1, unsigned int d2, 
			unsigned int d3, unsigned int d4, unsigned int d5, unsigned int d6) {
		assert(_n==7);			// Check dimensionality
		assert(d0<_dims.sizes.dim0);
		assert(d1<_dims.sizes.dim1);
		assert(d2<_dims.sizes.dim2);
		assert(d3<_dims.sizes.dim3);
		assert(d4<_dims.sizes.dim4);
		assert(d5<_dims.sizes.dim5);
		assert(d6<_dims.sizes.dim6);

		unsigned int  off =  d0 
				  + d1 * _offs.sizes.dim1 
				  + d2 * _offs.sizes.dim2 
				  + d3 * _offs.sizes.dim3 
				  + d4 * _offs.sizes.dim4 
				  + d5 * _offs.sizes.dim5 
				  + d6 * _offs.sizes.dim6;

		return _data[off];	
	}
	
	//! Array access operator for 8 dimensional array
	T & operator()(unsigned int d0, unsigned int d1, unsigned int d2, 
		unsigned int d3, unsigned int d4, unsigned int d5, unsigned int d6, 
		unsigned int d7) {
		assert(_n==8);			// Check dimensionality
		assert(d0<_dims.sizes.dim0);
		assert(d1<_dims.sizes.dim1);
		assert(d2<_dims.sizes.dim2);
		assert(d3<_dims.sizes.dim3);
		assert(d4<_dims.sizes.dim4);
		assert(d5<_dims.sizes.dim5);
		assert(d6<_dims.sizes.dim6);
		assert(d7<_dims.sizes.dim7);

		unsigned int  off =  d0 
				  + d1 * _offs.sizes.dim1 
				  + d2 * _offs.sizes.dim2 
				  + d3 * _offs.sizes.dim3 
				  + d4 * _offs.sizes.dim4 
				  + d5 * _offs.sizes.dim5 
				  + d6 * _offs.sizes.dim6 
				  + d7 * _offs.sizes.dim7;

		return _data[off];	
	}
	
	//! Array access operator for 9 dimensional array
	T & operator()(unsigned int d0, unsigned int d1, unsigned int d2, 
			unsigned int d3, unsigned int d4, unsigned int d5, unsigned int d6, 
			unsigned int d7, unsigned int d8) {
		assert(_n==9);			// Check dimensionality
		assert(d0<_dims.sizes.dim0);
		assert(d1<_dims.sizes.dim1);
		assert(d2<_dims.sizes.dim2);
		assert(d3<_dims.sizes.dim3);
		assert(d4<_dims.sizes.dim4);
		assert(d5<_dims.sizes.dim5);
		assert(d6<_dims.sizes.dim6);
		assert(d7<_dims.sizes.dim7);
		assert(d8<_dims.sizes.dim8);

		unsigned int  off =  d0 
				  + d1 * _offs.sizes.dim1 
				  + d2 * _offs.sizes.dim2 
				  + d3 * _offs.sizes.dim3 
				  + d4 * _offs.sizes.dim4 
				  + d5 * _offs.sizes.dim5 
				  + d6 * _offs.sizes.dim6 
				  + d7 * _offs.sizes.dim7
				  + d8 * _offs.sizes.dim8;

		return _data[off];	
	}

	//! Array access operator for 10 dimensional array
	T & operator()(unsigned int d0, unsigned int d1, unsigned int d2, 
			unsigned int d3, unsigned int d4, unsigned int d5, unsigned int d6, 
			unsigned int d7, unsigned int d8, unsigned int d9) {
		assert(_n==10);			// Check dimensionality
		assert(d0<_dims.sizes.dim0);
		assert(d1<_dims.sizes.dim1);
		assert(d2<_dims.sizes.dim2);
		assert(d3<_dims.sizes.dim3);
		assert(d4<_dims.sizes.dim4);
		assert(d5<_dims.sizes.dim5);
		assert(d6<_dims.sizes.dim6);
		assert(d7<_dims.sizes.dim7);
		assert(d8<_dims.sizes.dim8);
		assert(d9<_dims.sizes.dim9);

		unsigned int  off =  d0 
				  + d1 * _offs.sizes.dim1 
				  + d2 * _offs.sizes.dim2 
				  + d3 * _offs.sizes.dim3 
				  + d4 * _offs.sizes.dim4 
				  + d5 * _offs.sizes.dim5 
				  + d6 * _offs.sizes.dim6 
				  + d7 * _offs.sizes.dim7
				  + d8 * _offs.sizes.dim8
				  + d9 * _offs.sizes.dim9;

		return _data[off];	
	}
	
	///@} End of group started by \name
};


#endif // NARRAY_H