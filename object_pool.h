#pragma once
namespace Tmpl8 {
	template<typename T>
	class ObjectPool
	{
	public:
		vector<T> pool;
		//a deque is like a vector, except that you can add and remove on both sides.
		deque<T*> free_items;
		deque<T*> used_items;


		ObjectPool(int size, T example) {
			this->size = size;
			//allocate all the memory right away
			pool.reserve(size);

			//instantiate all the items in the pool
			fill(pool.begin(), pool.end(), example);

			//add all the free items to the free pool
			for (int i = 0; i < size; i++) {
				free_items.emplace_front(&pool[i]);
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
		/// <param name="pointer"> points to the item that can be freed</param>
		void free(T* pointer) {
			// we have to find the pointer in the deque, 
			// maybe should use a hashmap instead to circumvent this?
			for (int i = 0; i < used_items.size(); i++) {
				if (used_items[i] == pointer) {
					// when we find the pointer, remove it from the used items 
					// and add it to the unused items
					used_items.erase(used_items.begin() + i);
					free_items.emplace_front(pointer);
					//no need to loop over the rest of the pointers after this.
					return;
				}
				//throw if we don't find the pointer, because something definitely went wrong...
				throw runtime_error(string("tried to free a pointer that wasn't in use..."));
			}
		};
		
		/// <summary>
		/// throw the entire object pool in the output for debugging.
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

