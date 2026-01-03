#include<iostream>

using namespace std;

template<typename T>
class array
{

    public:
    int size;
    int max_size;
    T *arr;
    Array(int msize)
    {
        this->max_size = msize;
        arr = new T[msize];
        size = 0;
    }
    void add(T data)
    {
        try
        {
            arr[size++] = data;
        }
        catch(arrayIndexOutOfBoundsException e)
        {
            cout << "Array Index Out of Bounds Exception" << endl;
        }
    }
    T operator[](int index)
    {
        try
        {
            if(index >= size)
            {
                throw arrayIndexOutOfBoundsException();
            }
            return arr[index];
        }
        catch(arrayIndexOutOfBoundsException e)
        {
            cout << "Array Index Out of Bounds Exception" << endl;
            T temp;
            return temp;
        }
    }

};