#pragma once

#include <Windows.h>

struct Clock
{
	LARGE_INTEGER li;
	double frequency;
	__int64 ticks;

	void Start()
	{
		QueryPerformanceFrequency(&li);
		frequency = (double)li.QuadPart;

		QueryPerformanceCounter(&li);
		ticks = li.QuadPart;
	}

	double Time()
	{
		QueryPerformanceCounter(&li);

		double elapsed = (double)(li.QuadPart - ticks) / frequency;
		ticks = li.QuadPart;

		return elapsed;
	}

	__int64 GetTicks()
	{
		QueryPerformanceCounter(&li);
		return li.QuadPart;
	}

	double TicksToSeconds(__int64 t)
	{
		return (double)t / frequency;
	}
};

struct BUFFER
{
	BYTE* pBase = NULL;
	DWORD dwSize = 0;
	DWORD dwPos = 0;

	BUFFER() {}
	BUFFER(DWORD length)
	{
		pBase = new BYTE[length];
		dwSize = length;
	}
	~BUFFER() { delete[] pBase; }
};

template <typename T>
struct LNode
{
	T* pData = NULL;
	LNode<T>* pNext = NULL;
};

template <typename T>
struct LList
{
	LNode<T>* pBase = NULL;
	LNode<T>* pEnd = NULL;
	LNode<T>* pCurr = NULL;
	LNode<T>** ppPrev = &pBase;

	T* LList<T>::Begin()
	{
		pCurr = pBase;
		ppPrev = &pBase;
		return pBase != NULL ? pBase->pData : NULL;
	}

	T* LList<T>::At()
	{
		return pCurr != NULL ? pCurr->pData : NULL;
	}

	void LList<T>::Next()
	{
		if (!pCurr)
			return;
		ppPrev = &pCurr->pNext;
		pCurr = pCurr->pNext;
	}

	void LList<T>::Remove(bool del)
	{
		if (!pCurr)
			return;
		LNode<T>* pNext = pCurr->pNext;
		if (del)
			delete pCurr->pData;
		delete pCurr;
		pCurr = pNext;
		*ppPrev = pNext;
		if (!pNext)
			pEnd = !pBase ? NULL : (LNode<T>*)(ppPrev - 1);
	}

	void LList<T>::RemoveAll()
	{
		if (!pBase)
			return;
		pCurr = pBase;
		while (pCurr != NULL)
		{
			LNode<T>* pNext = pCurr->pNext;
			delete pCurr->pData;
			delete pCurr;
			pCurr = pNext;
		}
		pBase = NULL;
		pEnd = NULL;
		pCurr = NULL;
		ppPrev = &pBase;
	}

	void LList<T>::Push(T* pData)
	{
		LNode<T>* pNode = new LNode<T>;
		pNode->pData = pData;
		pNode->pNext = pBase;
		if (!pBase)
			pEnd = pNode;
		pBase = pNode;
	}

	void LList<T>::Queue(T* pData)
	{
		LNode<T>* pNode = new LNode<T>;
		pNode->pData = pData;
		pNode->pNext = NULL;
		if (!pEnd)
			pBase = pNode;
		else
			pEnd->pNext = pNode;
		pEnd = pNode;
	}
};