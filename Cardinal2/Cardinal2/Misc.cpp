#include "stdafx.h"
#include "Misc.h"

//LList<T>

template <typename T>
T* LList<T>::Begin()
{
	ppPrev = &pBase;
	pCurr = pBase;
}

template <typename T>
T* LList<T>::At()
{
	if (!pCurr)
		return NULL;
	return pCurr->pData;
}

template <typename T>
void LList<T>::Next()
{
	if (!pCurr || pCurr == pEnd)
		return;
	ppPrev = &pCurr->pNext;
	pCurr = pCurr->pNext;
}

template <typename T>
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
}

template <typename T>
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
}

template <typename T>
void LList<T>::Push(T* pData)
{
	LNode<T>* pNode = new LNode<T>;
	pNode->pData = pData;
	pNode->pNext = pBase;
	if (!pBase)
		pEnd = pNode;
	pBase = pNode;
}

template <typename T>
void LList<T>::Queue(T* pData)
{
	LNode<T>* pNode = new LNode<T>;
	pNode->pData = pData;
	pEnd->pNext = pNode;
	pEnd = pNode;
}

template <typename T>
T* LList<T>::Pop()
{
	if (!pBase)
		return NULL;
	T* pData = pBase->pData;
	pBase = pBase->pNext;
	if (!pBase)
		pEnd = NULL;
	return pData;
}

//GameClock

void GameClock::Start()
{
	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);
	frequency = (double)li.QuadPart;

	QueryPerformanceCounter(&li);
	ticks = li.QuadPart;
}

double GameClock::Time()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);

	double elapsed = (double)(li.QuadPart - ticks) / frequency;
	ticks = li.QuadPart;

	return elapsed;
}