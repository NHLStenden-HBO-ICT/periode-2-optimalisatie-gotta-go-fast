#pragma once
namespace Tmpl8 {
	template<typename T>
	class ObjectPool
	{
		//this class is header only because template classes don't play nice with .cpp files
		//see https://stackoverflow.com/questions/495021/why-can-templates-only-be-implemented-in-the-header-file

	public:
		//all our shit's public because we need access to these pools
		//modify the pools in any other way but the profided methods at your own peril.
		vector<T> pool;
		//a deque is like a vector, except that you can add and remove on both sides.
		deque<T*> free_items;
		deque<T*> used_items;


		ObjectPool(int size, T example) {
			this->size = size;
			//allocate all the memory right away
			pool.reserve(size);

			//instantiate all the items in the pool
			//and add all the free items to the free pool
			for (int i = 0; i < size; i++) {
				pool.emplace_back(example);
				free_items.emplace_front(&(pool[i]));
			}
		};
		
		/// <summary>
		/// Gets an unused item from the object pool. 
		/// It returns a pointer to that item 
		/// and keeps track internally that this item is currently in use
		/// </summary>
		/// <returns>Pointer to an unused item</returns>
		T* get() {
			//get the index of the last free item in the deque
			size_t index = free_items.size() - 1;
			//get the pointer at that index
			T* ptr = free_items[index];
			//remove that pointer from the deque
			free_items.pop_back();
			//add that pointer to the start of the used_items deque
			used_items.emplace_front(ptr);
			
			return ptr;
		};

		/// <summary>
		/// Frees up an item that's no longer used.
		/// </summary>
		/// <param name="iterator"> points to the item that will be freed</param>
		typename deque<T*>::iterator free(typename deque<T*>::iterator it) {
			//simply add the pointer at the index to the free items pool and then remove it from used items
			//we do this with an iterator, though doing it by index would also be possible
			//when I made this I was of the opinion that this was neater because it's now container agnositic. 
			//If we want to switch from a deque to something else nothing needs to change
			free_items.emplace_front(*it);
			return used_items.erase(it);
			
		};
		
		/// <summary>
		/// turn the object pool into a string for some bad debug practices ;).
		/// </summary>
		/// <returns>a string containing information about the object pool</returns>
		string to_string() {
			stringstream str;
			str << "object pool {" << endl;
			str << "  size: " << this->size << endl;
			str << "  free: " << free_items.size() << endl;
			str << "  used: " << used_items.size() << endl;
			str << "}" << endl;

			return str.str();
		}
	private:
		size_t size;
		
	};
}

