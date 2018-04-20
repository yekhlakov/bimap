#pragma once
#include <map>
#include <unordered_map>
#include <type_traits>

namespace maxy
{
	/**
	 * _bimap is a map of unique key=>value pairs
	 * It allows accessing elements by key and by value
	 */
	template <typename Kt, typename Vt, template<typename...> typename C>
	class _bimap
	{
		// values are kept as pairs
		using storage_type = std::pair<Kt, Vt>;

		// helper types for internal maps
		template <typename T>
		using map_type = C<T, storage_type *>;
		using key_map_type = map_type<Kt>;
		using value_map_type = map_type<Vt>;
		
		// forward and backward maps
		key_map_type forward;
		value_map_type backward;

		// templated getter for internal maps
		template<typename T> map_type<T> & get_map () { get_map((T*)nullptr); }
		map_type<Kt> & get_map (Kt *) { return forward; }
		map_type<Vt> & get_map (Vt *) { return backward; }

		// put a pair into the _bimap and disregard any possible conflicts
		storage_type * raw_set (Kt key, Vt value)
		{
			auto v = new std::pair<Kt, Vt> (key, value);
			return raw_set (v);
		}
		storage_type * raw_set (storage_type * v)
		{
			forward[v->first] = v;
			backward[v->second] = v;
			return v;
		}

		public:

		// default constructor
		_bimap () = default;
		// destructor
		~_bimap () { for (auto p : forward) delete p.second; }

		/*
		 * Force set k=>v pair overwriting old assignments
		 */
		storage_type * set (Kt key, Vt value)
		{
			return set (new std::pair<Kt, Vt> (key, value));
		}

		storage_type * set (storage_type * p)
		{
			auto x = forward.find (p->first);
			storage_type * s0 = nullptr, * s1 = nullptr;

			if (x != forward.end ()) // key exists
			{
				s0 = x->second;
				forward.erase (p->first);
			}

			auto y = backward.find (p->second);
			if (y != backward.end ()) // value exists
			{
				s1 = y->second;
				backward.erase (p->second);
			}

			if (s0) delete s0;
			if (s1 && s1 != s0) delete s1;

			return raw_set (p);
		}

		/**
		 * Attempt to insert a new pair, return iterators pointing to inserted elements and true
		 * OR return iterator(s) to existing elements and false
		 */
		std::pair<storage_type *, bool> insert (Kt key, Vt value)
		{
			auto k = forward.find (key);
			if (k != forward.end ())
				return std::make_pair (k->second, false);

			auto v = backward.find (value);
			if (v != backward.end())
				return std::make_pair(v->second, false);

			return std::make_pair (raw_set (key, value), true);
		}

		// remove by key or value; return true if removed something
		template<typename T> 
		bool _erase (T t)
		{
			auto k = get_map<T> ().find (t);
			if (k == get_map<T> ().end ()) return false;
			auto p = k->second;
			forward.erase (p->first);
			backward.erase (p->second);
			delete p;
			return true;
		}

		bool erase (Kt k) { return _erase (k); }
		bool erase (Vt v) { return _erase (v); }

		// Access element by key or value; return pointer to storage if found or null if not found
		template<typename T>
		const storage_type * _access (const T & k)
		{
			auto p = get_map<T>().find (k);
			if (p == get_map<T>().end ()) return nullptr;
			return p->second;
		}

		const storage_type * operator[] (const Kt & k) { return _access (k); }
		const storage_type * operator[] (const Vt & k) { return _access (k); }


		// Access element by value; return pointer to storage if found or null if not found
		/*
		const storage_type * operator[] (const Vt & v)
		{
			auto p = backward.find (v);
			if (p == backward.end ()) return nullptr;
			return p->second;
		}
		*/
		// Iterator
		template<typename T>
		class iterator
		{
			// helper type
			using map_iterator_type = typename map_type<T>::iterator;

			// owning map
			_bimap & map;
			// iterator of corresponding map
			map_iterator_type it;

			public:
			// begin constructor
			iterator (_bimap & m) : map{m} { it = m.get_map<T>().begin (); }
			// 'end' constructor
			iterator (_bimap & m, map_iterator_type i) : map{m}, it{i} {}
			// copy constructor is default
			iterator (iterator & o) = default;
			iterator (iterator && o) = default;
			// destructor is default
			~iterator () = default;

			// we need at least increment
			iterator & operator++ () { ++it; return *this; }
			iterator operator++ (int) { auto x = *this; ++it; return x; }
			// and comparison
			bool operator== (const iterator & o) { return &map == &o.map && it == o.it; }
			bool operator!= (const iterator & o) { return &map != &o.map || it != o.it; }

			// define decrement if the container allows
			template
			<
			    typename = std::enable_if
			    <
				std::is_same
				<
				    typename std::iterator_traits<typename map_type<T>::iterator>::iterator_category,
				    std::bidirectional_iterator_tag
				>::value,
				bool
			    >
			>
			iterator & operator--() { --it; return *this; };

			template
			<
			    typename = std::enable_if
			    <
				std::is_same
				<
				    typename std::iterator_traits<typename map_type<T>::iterator>::iterator_category,
				    std::bidirectional_iterator_tag
				>::value,
				bool
			    >
			>
			iterator operator--(int) { auto x = *this; --it; return *this; };

			// element access; iterator gives const access to pair storage
			storage_type * const operator* () const { return it->second; }
		};

		// Reverse iterator
		template<typename T, typename R = typename map_type<T>::reverse_iterator>
		class reverse_iterator
		{
			// helper type
			using map_iterator_type = R;

			// owning map
			_bimap & map;
			// iterator of corresponding map
			map_iterator_type it;

			public:
			// begin constructor
			reverse_iterator (_bimap & m) : map{m} { it = m.get_map<T> ().rbegin (); }
			// 'end' constructor
			reverse_iterator (_bimap & m, map_iterator_type i) : map{m}, it{i} {}
			// copy constructor is default
			reverse_iterator (reverse_iterator & o) = default;
			reverse_iterator (reverse_iterator && o) = default;
			// destructor is default
			~reverse_iterator () = default;

			// we need at least increment
			reverse_iterator & operator++ () { ++it; return *this; }
			reverse_iterator operator++ (int) { auto x = *this; ++it; return x; }
			// and comparison
			bool operator== (const reverse_iterator & o) { return &map == &o.map && it == o.it; }
			bool operator!= (const reverse_iterator & o) { return &map != &o.map || it != o.it; }

			// reverse iterator allows decrements
			reverse_iterator & operator--() { --it; return *this; };
			reverse_iterator operator--(int) { auto x = *this; --it; return *this; };

			// element access; iterator gives const access to pair storage
			storage_type * const operator* () const { return it->second; }
		};

		// default iteration is by key

		// begin iterator by key
		iterator<Kt> begin () { return iterator<Kt>{*this}; }
		// end iterator by key
		iterator<Kt> end () { return iterator<Kt>{*this, forward.end ()}; }

		// begin iterator by value
		iterator<Vt> vbegin () { return iterator<Vt>{*this}; }
		// end iterator by value
		iterator<Vt> vend () { return iterator<Vt>{*this, backward.end ()}; }

		//reverse_iterator<Kt> rbegin () { return reverse_iterator<Kt>{*this}; }
		//reverse_iterator<Kt> rend () { return reverse_iterator<Kt>{*this, forward.rend ()}; }

	};

	template <typename Kt, typename Vt>
	using unordered_bimap = _bimap<Kt, Vt, std::unordered_map>;

	template <typename Kt, typename Vt>
	using bimap = _bimap<Kt, Vt, std::map>;

}
