

#ifndef __p2spVersion_h__
#define __p2spVersion_h__

#pragma once
#include <string>

namespace p2sp4 
{
	class version 
	{
	public:
		version() : _hi(0), _major(0), _minor(0), _least(0) 
		{

		}
		
		version(size_t hi, size_t major, size_t minor, size_t least) 
			: _hi(hi), _major(major), _minor(minor), _least(least)
		{

		}

		explicit version(size_t least) : _hi(0), _major(0), _minor(0), _least(least)
		{

		}

		version(const version& rhs) 
			: _hi(rhs._hi), _major(rhs._major), _minor(rhs._minor), _least(rhs._least)
		{

		}

		version(const std::string& ver, const std::string& split = "-_.");

		void assign(size_t hi, size_t major, size_t minor, size_t least) 
		{
			_hi = hi;
			_major = major;
			_minor = minor;
			_least = least;
		}

		void assign(const std::string& ver, const std::string& split = "-_.");

		inline version& operator = ( const version& rhs )
		{
			_hi = rhs._hi;
			_major = rhs._major;
			_minor = rhs._minor;
			_least = rhs._least;
			return *this;
		}

		inline bool operator < ( const version& rhs ) const
		{
			if(_hi < rhs._hi) return true;
			else if(_hi > rhs._hi) return false;  

			if(_major < rhs._major) return true; 
			else if(_major > rhs._major) return false;  

			if(_minor < rhs._minor) return true;
			else if(_minor > rhs._minor) return false; 

			if(_least < rhs._least) return true; 
			else if(_least > rhs._least) return false;

			return false;
		}

		inline bool operator > ( const version& rhs ) const
		{
			if(_hi > rhs._hi) return true;
			else if(_hi < rhs._hi) return false;

			if(_major > rhs._major) return true;
			else if(_major < rhs._major) return false;

			if(_minor > rhs._minor) return true;
			else if(_minor < rhs._minor) return false;

			if(_least > rhs._least) return true;
			else if(_least < rhs._least) return false;

			return false;
		}

		inline bool operator == ( const version& rhs ) const
		{
			return ( _hi == rhs._hi && _major == rhs._major 
				&& _minor == rhs._minor && _least == rhs._least );
		}

		inline bool operator >= (const version& rhs ) const
		{
			if(*this > rhs) return true;
			if(*this == rhs) return true;
			return false;
		}

		inline bool operator <= (const version& rhs ) const
		{
			if(*this < rhs) return true;
			if(*this == rhs) return true;
			return false;
		}

		inline bool operator != ( const version& rhs ) const
		{
			return !(*this == rhs);
		}

		std::string to_string() const;

		bool is_zero() const {
			return _hi == 0 && _major == 0 && _minor == 0 && _least == 0;
		}

		static version zeroVersion;

	private:
		size_t _hi, _major, _minor, _least;
	};
}

#endif //
