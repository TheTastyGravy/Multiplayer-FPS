#pragma once

/// <summary>
/// A collection of constant size that will overwrite the first element when full
/// </summary>
template<class Type>
class RingBuffer
{
public:
	RingBuffer(size_t size = 30);
	~RingBuffer();


	/// <summary>
	/// Get an element relitive to the first element
	/// </summary>
	Type& operator[](size_t index);
	/// <summary>
	/// Get an element relitive to the first element
	/// </summary>
	const Type& operator[](size_t index) const;

	/// <summary>
	/// Push an element onto the end of the buffer. If the buffer is full, the first element will be overwritten
	/// </summary>
	void push(Type element);

	/// <summary>
	/// Returns the number of elements being used
	/// </summary>
	size_t getSize() const;


private:
	Type* buffer;

	const size_t size;

	size_t start;	// Read pos
	size_t end;		// Write pos

	bool isEmpty;
};