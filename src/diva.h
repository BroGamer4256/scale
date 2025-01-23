#pragma once
namespace diva {
struct Vec2 {
	f32 x;
	f32 y;

	Vec2 () {
		this->x = 0;
		this->y = 0;
	}

	Vec2 (f32 x, f32 y) {
		this->x = x;
		this->y = y;
	}

	Vec2 operator+ (Vec2 other) { return Vec2 (this->x + other.x, this->y + other.y); }
	Vec2 operator- (Vec2 other) { return Vec2 (this->x - other.x, this->y - other.y); }
	Vec2 operator* (Vec2 other) { return Vec2 (this->x * other.x, this->y * other.y); }
	Vec2 operator/ (Vec2 other) { return Vec2 (this->x / other.x, this->y / other.y); }
	Vec2 operator+ (f32 offset) { return Vec2 (this->x + offset, this->y + offset); }
	Vec2 operator* (f32 scale) { return Vec2 (this->x * scale, this->y * scale); }
	Vec2 operator/ (f32 scale) { return Vec2 (this->x / scale, this->y / scale); }
};

struct Vec3 {
	f32 x;
	f32 y;
	f32 z;

	Vec3 () {
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}

	Vec3 (f32 x, f32 y, f32 z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	Vec3 operator+ (Vec3 other) { return Vec3 (this->x + other.x, this->y + other.y, this->z + other.z); }
	Vec3 operator- (Vec3 other) { return Vec3 (this->x - other.x, this->y - other.y, this->z - other.z); }
	Vec3 operator* (Vec3 other) { return Vec3 (this->x * other.x, this->y * other.y, this->z * other.z); }
	Vec3 operator/ (Vec3 other) { return Vec3 (this->x / other.x, this->y / other.y, this->z / other.z); }
	Vec3 operator* (f32 scale) { return Vec3 (this->x * scale, this->y * scale, this->z * scale); }
	Vec3 operator/ (f32 scale) { return Vec3 (this->x / scale, this->y / scale, this->z / scale); }
};

struct Vec4 {
	f32 x;
	f32 y;
	f32 z;
	f32 w;

	Vec4 () {
		this->x = 0;
		this->y = 0;
		this->z = 0;
		this->w = 0;
	}

	Vec4 (f32 x, f32 y, f32 z, f32 w) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	bool contains (Vec2 location) { return location.x > this->x && location.x < this->z && location.y > this->y && location.y < this->w; }
};

FUNCTION_PTR_H (void *, operatorNew, u64);
FUNCTION_PTR_H (void *, operatorDelete, void *);
struct string;
FUNCTION_PTR_H (void, FreeString, string *);
template <typename T>
T *
allocate (u64 count) {
	return (T *)(operatorNew (count * sizeof (T)));
}

inline void
deallocate (void *p) {
	operatorDelete (p);
}

#pragma pack(push, 8)
struct string {
	union {
		char data[16];
		char *ptr;
	};
	u64 length;
	u64 capacity;

	char *c_str () {
		if (this->capacity > 15) return this->ptr;
		else return this->data;
	}

	string () {
		memset (this->data, 0, sizeof (this->data));
		this->length   = 0;
		this->capacity = 0x0F;
	}
	string (const char *cstr) {
		u64 len = strlen (cstr) + 1;
		if (len > 15) {
			u64 new_len = len | 0xF;
			this->ptr   = allocate<char> (new_len);
			strcpy (this->ptr, cstr);
			this->length   = len - 1;
			this->capacity = new_len;
		} else {
			strcpy (this->data, cstr);
			this->length   = len - 1;
			this->capacity = 15;
		}
	}
	string (char *cstr) {
		u64 len = strlen (cstr) + 1;
		if (len > 15) {
			u64 new_len = len | 0xF;
			this->ptr   = allocate<char> (new_len);
			strcpy (this->ptr, cstr);
			this->length   = len - 1;
			this->capacity = new_len;
		} else {
			strcpy (this->data, cstr);
			this->length   = len - 1;
			this->capacity = 15;
		}
	}

	~string () { FreeString (this); }

	bool operator== (string &rhs) {
		if (!this->c_str () || !rhs.c_str ()) return false;
		return strcmp (this->c_str (), rhs.c_str ()) == 0;
	}
	bool operator== (char *rhs) {
		if (!this->c_str () || !rhs) return false;
		return strcmp (this->c_str (), rhs) == 0;
	}
	auto operator<=> (string &rhs) {
		if (!this->c_str () || !rhs.c_str ()) return 1;
		return strcmp (this->c_str (), rhs.c_str ());
	}
	auto operator<=> (char *rhs) {
		if (!this->c_str () || !rhs) return 1;
		return strcmp (this->c_str (), rhs);
	}

	void extend (size_t len) {
		size_t extension = this->length + 1 + len;
		if (extension <= this->capacity) return;

		size_t new_capacity = extension | 0x0F;
		auto old            = this->c_str ();
		auto new_ptr        = allocate<char> (new_capacity);
		strcpy (new_ptr, old);

		if (this->capacity >= 15) deallocate (this->ptr);
		this->ptr      = new_ptr;
		this->capacity = new_capacity;
	}

	void operator+ (string &rhs) {
		this->extend (rhs.length);
		strcpy (&this->c_str ()[this->length], rhs.c_str ());
		this->length += rhs.length;
	}
	void operator+ (char *rhs) {
		this->extend (strlen (rhs));
		strcpy (&this->c_str ()[this->length], rhs);
		this->length += strlen (rhs);
	}
	void operator+ (const char *rhs) {
		this->extend (strlen (rhs));
		strcpy (&this->c_str ()[this->length], rhs);
		this->length += strlen (rhs);
	}
	void operator+ (char rhs) {
		this->extend (1);
		this->c_str ()[this->length] = rhs;
		this->length += 1;
	}
};

template <typename T>
struct listElement {
	listElement<T> *next;
	listElement<T> *previous;
	T current;

	bool operator== (const listElement<T> &rhs) { return this->current == rhs->current; }
	bool operator== (const T &rhs) { return this->current == rhs; }
};

template <typename T>
struct list {
	listElement<T> *root;
	u64 length;

	listElement<T> *begin () { return this->length ? this->root->next : this->root; }
	listElement<T> *end () { return this->root; }
};

template <typename K, typename V>
struct pair {
	K key;
	V value;
};

template <typename K, typename V>
struct mapElement {
	mapElement<K, V> *left;
	mapElement<K, V> *parent;
	mapElement<K, V> *right;
	bool isBlack;
	bool isNull;

	pair<K, V> pair;

	mapElement<K, V> *next () {
		auto ptr = this;
		if (ptr->right->isNull) {
			while (!ptr->parent->isNull && ptr == ptr->parent->right)
				ptr = ptr->parent;
			ptr = ptr->parent;
		} else {
			ptr = ptr->right;
			while (!ptr->left->isNull)
				ptr = ptr->left;
		}
		return ptr;
	}

	K *key () { return &this->pair.key; }

	V *value () { return &this->pair.value; }
};

template <typename K, typename V>
struct map {
	mapElement<K, V> *root;
	u64 length;

	map () {
		this->root          = allocate<mapElement<K, V>> (1);
		this->root->left    = this->root;
		this->root->parent  = this->root;
		this->root->right   = this->root;
		this->root->isBlack = true;
		this->root->isNull  = true;
		this->length        = 0;
	}

	~map () {
		for (auto it = this->begin (); it != this->end (); it = it->next ())
			deallocate (it);
		deallocate (this->root);
	}

	std::optional<V *> find (K key) {
		auto ptr = this->root->parent;
		while (!ptr->isNull)
			if (key == ptr->pair.key) return std::optional (ptr->value ());
			else if (key > ptr->pair.key) ptr = ptr->right;
			else if (key < ptr->pair.key) ptr = ptr->left;
		return std::nullopt;
	}

	mapElement<K, V> *begin () { return this->length ? this->root->left : this->root; }
	mapElement<K, V> *end () { return this->root; }
};

template <typename T>
struct vector {
	T *first;
	T *last;
	void *capacity_end;

	vector () {}
	vector (u64 n) {
		this->first        = allocate<T> (n);
		this->last         = this->first;
		this->capacity_end = (void *)((u64)this->first + (n * sizeof (T)));
	}

	~vector () { deallocate (this->first); }

	std::optional<T *> at (u64 index) {
		if (index >= this->length ()) return std::nullopt;
		return std::optional (&this->first[index]);
	}

	void push_back (T value) {
		if (this->remaining_capcity () > 0) {
			this->first[this->length ()] = value;
			this->last++;
			return;
		}

		u64 new_length = this->length () + (this->length () / 2);
		T *new_first   = allocate<T> (new_length);
		u64 old_length = (u64)this->last - (u64)this->first;
		memcpy (new_first, this->first, old_length);
		deallocate (this->first);

		this->first        = new_first;
		this->last         = (T *)((u64)new_first + old_length);
		this->capacity_end = (void *)((u64)new_first + (new_length * sizeof (T)));
		this->push_back (value);
	}

	std::optional<T *> find (const std::function<bool (T *)> &func) {
		for (auto it = this->begin (); it != this->end (); it++)
			if (func (it)) return std::optional (it);
		return std::nullopt;
	}

	u64 length () { return ((u64)this->last - (u64)this->first) / sizeof (T); }
	u64 capacity () { return ((u64)this->capacity_end - (u64)this->first) / sizeof (T); }
	u64 remaining_capcity () { return this->capacity () - this->length (); }

	T *begin () { return this->first; }
	T *end () { return this->last; }
};
#pragma pack(pop)

extern vector<string> *romDirs;
FUNCTION_PTR_H (bool, ResolveFilePath, string *from, string *out);
FUNCTION_PTR_H (bool, FileRequestLoad, void *fileHandler, char *file, bool);
FUNCTION_PTR_H (bool, FileCheckNotReady, void *fileHandler);
FUNCTION_PTR_H (const void *, FileGetData, void *fileHandler);
FUNCTION_PTR_H (size_t, FileGetSize, void *fileHandler);
FUNCTION_PTR_H (void, FreeFileHandler, void *fileHandler);
} // namespace diva
