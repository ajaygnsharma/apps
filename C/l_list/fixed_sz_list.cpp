//////////////////////////////////////////////////////////////////////////////
//                                                                           /
// All information contained herein is, and remains the property of Western  /
// Digital Corporation. The intellectual and technical concepts contained    /
// herein are proprietary to Western Digital Corporation and may be covered  /
// by U.S. and Foreign Patents, patents in process, and are protected by     /
// trade secret or copyright law.                                            /
//                                                                           /
// Dissemination of this information or reproduction of this material is     /
// strictly forbidden unless prior written permission is obtained from:      /
//                                                                           /
//      Western Digital Corporation.                                         /
//      Solid State Storage Business Unit                                    /
//      Western Digital                                                      /
//      3355 Michelson Drive, Suite 100                                      /
//      Irvine, California 92612                                             /
//                                                                           /
// Copyright (c) 2011 Western Digital Corporation.                           /
// All Rights Reserved Worldwide.                                            /
//                                                                           /
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//      Project: SabreModel                                                 //
//                                                                          //
//     Released: 09/25/2012                                                 //
//                                                                          //
//  Description: help utilities                                             //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <boost/thread/condition.hpp>
#include <iostream>
using namespace std;

#include <set>
#include <algorithm>
#include <boost/lambda/lambda.hpp>
#include <boost/bind.hpp>

typedef     uint64_t            UINT64;
typedef     uint32_t            UINT32;
typedef     uint16_t            UINT16;
typedef     unsigned char       UINT8;
typedef     unsigned char       UCHAR;
typedef     int64_t             INT64;
typedef     int32_t             INT32;
typedef     int16_t             INT16;
typedef     char                INT8;
typedef     char                CHAR;



/**
 * \brief the base exception for SystemC SabreModel project
 */
class SC_SM_Exception: public std::exception
{
public:
    SC_SM_Exception(std::string error) :
            error(error)
    {
    }
    virtual ~SC_SM_Exception() throw ()
    {
    }

    const char* what() const throw ()
    {
        return error.c_str();
    }

private:
    std::string error;
};

template<typename Node>
UINT32& NextNodePos(Node* Base, const UINT32 Pos, int TYPE = 1){
    return (Base + Pos)->NextLink;
}

/*
 template<typename Node>
 void SetNextPos(Node* Base, const UINT32 Pos, const UINT32 NextPos) {
 NextNodePos(Base, Pos) = NextPos;
 }

 template<typename Node>
 UINT32 GetNextPos(Node* Base, const UINT32 Pos) {
 return NextNodePos(Base, Pos);
 }
 */

template<typename Node, int TYPE = 1>
class LinkedList
{

public:
    typedef void (*NodeFunc)(Node*);

public:
    LinkedList(Node*& Base, UINT32& Header, UINT32& Tail) :
            Base(Base), Header(Header), Tail(Tail)
    {
    }

    virtual ~LinkedList()
    {
    }

    UINT32 PopHeader(UINT32 Size = 1)
    {
        UINT32 LastPos;

        for (UINT32 i = 0; i < Size; i++)
        {
            LastPos = Header;
            Header = GetNextPos(Header);
        }

        return LastPos;
    }

    void PushTail(const INT32 NewPos)
    {
        if (IsEmpty())
        {
            Header = NewPos;
            Tail = NewPos;
        }
        else
        {
            SetNextPos(Tail, NewPos);
            Tail = NewPos;
        }
    }

    void PushTail(LinkedList<Node, TYPE>& NewList)
    {
        if (IsEmpty())
        {
            Header = NewList.Header;
            Tail = NewList.Tail;
        }
        else
        {
            SetNextPos(Tail, NewList.Header);
            Tail = (UINT32) NewList.Tail;
        }
    }

    void SetNextPos(const UINT32 Pos, const UINT32 NextPos)
    {
        NextNodePos(Base, Pos, TYPE) = NextPos;
    }

    UINT32 GetNextPos(const UINT32 Pos)
    {
        return NextNodePos(Base, Pos, TYPE);
    }

    virtual UINT32 GetCount() = 0;
    virtual bool IsEmpty() = 0;

    template<typename Function>
    void ForEach(Function f)
    {
        if (IsEmpty())
        {
            return;
        }

        UINT32 Cursor = Header;
        while (Cursor != Tail)
        {
            f(Cursor, Base + Cursor);
            Cursor = GetNextPos(Cursor);
        }

        // The tail node
        f(Cursor, Base + Cursor);
    }

    Node*& Base;
    UINT32& Header;
    UINT32& Tail;
};

template<typename Node, int TYPE = 1>
class CheckEmptyLinkedList: public LinkedList<Node>
{

private:
    typedef LinkedList<Node> Parent;

public:
    CheckEmptyLinkedList(Node*& Base, UINT32& Header, UINT32& Tail, bool& Empty) :
            LinkedList<Node>(Base, Header, Tail), Empty(Empty)
    {
    }

    virtual ~CheckEmptyLinkedList()
    {
    }

    UINT32 PopHeader()
    {
        if (Parent::Header == Parent::Tail)
        {
            Empty = 1;
            return Parent::Header;
        }

        return Parent::PopHeader();
    }

    void PushTail(const INT32 NewPos)
    {
        if (Empty)
        {
            Parent::Header = NewPos;
            Parent::Tail = NewPos;
            Empty = 0;
        }
        else
        {
            Parent::PushTail(NewPos);
        }
    }

    void PushTail(CheckEmptyLinkedList<Node, TYPE> NewList)
    {
        if (NewList.Empty)
        {
            return;
        }

        Parent::PushTail(NewList);
        Empty = 0;
    }

    virtual UINT32 GetCount()
    {
        UINT32 count = 0;
        if (Empty)
        {
            return count;
        }

        Parent::ForEach(++boost::lambda::var(count));

        return count;
    }

    virtual bool IsEmpty()
    {
        return (Empty);
    }

    bool& Empty;
};

template<typename Node, int TYPE = 1>
class CountedLinkedList: public LinkedList<Node>
{

private:
    typedef LinkedList<Node> Parent;

public:
    CountedLinkedList(Node*& Base, UINT32& Header, UINT32& Tail, UINT32& Count) :
            LinkedList<Node>(Base, Header, Tail), Count(Count)
    {
    }

    virtual ~CountedLinkedList()
    {
    }

    void Init(const UINT32 Count)
    {
        Parent::Header = 0;
        Parent::Tail = Count - 1;
        for (UINT32 i = Parent::Header; i < Parent::Tail; i++)
        {
            Parent::SetNextPos(i, i + 1);
        }
        this->Count = Count;
    }

    UINT32 PopHeader(UINT32 Size = 1)
    {
        if (Count < Size)
            return (UINT32) -1;

        Count -= Size;

        return Parent::PopHeader(Size);
    }

    void PushTail(const INT32 NewPos)
    {
        Parent::PushTail(NewPos);
        Count++;
    }

    void PushTail(CheckEmptyLinkedList<Node, TYPE> NewList)
    {
        Parent::PushTail(NewList);
        Count += NewList.GetCount();
    }

    virtual UINT32 GetCount()
    {
        return Count;
    }

    virtual bool IsEmpty()
    {
        return (Count == 0);
    }

    UINT32& Count;
};

// An implementation of doubled linked list based on array
// Node should have following data member
// FwdLink,  position of previous node
// BackLink, postion of next node
//
template<typename NODE>
class DoubleLinkedList
{
public:
    DoubleLinkedList(NODE*& Base, UINT32& Header, bool& Empty) :
            Base(Base), Header(Header), Empty(Empty), Count(0)
    {
    }

    virtual ~DoubleLinkedList()
    {
    }

    void PushTail(const UINT32 Pos)
    {

        if (Empty)
        {
            Empty = 0;
            Header = Pos;

            (Base + Pos)->FwdLink = Pos;
            (Base + Pos)->BackLink = Pos;
        }
        else
        {
            UINT32 Tail = (Base + Header)->BackLink;

            (Base + Pos)->FwdLink = Header;
            (Base + Pos)->BackLink = Tail;

            (Base + Header)->BackLink = Pos;
            (Base + Tail)->FwdLink = Pos;
        }

        Count++;
    }

    void RemoveAt(const UINT32 Pos)
    {
        assert(Count > 0);

        NODE* PosNode = Base + Pos;

        if (Count == 1)
        {
            assert(Pos == Header);
            Empty = 1;
        }
        else
        {
            (Base + PosNode->FwdLink)->BackLink = PosNode->BackLink;
            (Base + PosNode->BackLink)->FwdLink = PosNode->FwdLink;

            if (Pos == Header)
            {
                Header = PosNode->FwdLink;
            }
        }

        Count--;
    }

    void Clear()
    {
        Empty = 1;
        Count = 0;
    }

    UINT32 PopHeader()
    {
        UINT32 Ret = Header;
        RemoveAt(Header);
        return Ret;
    }

    UINT32 PeekHeader()
    {
        UINT32 Ret = Header;
        return Ret;
    }

    bool PeekNext(const UINT32 Pos, UINT32& Next)
    {
        UINT32 Ret = Header;

        NODE* node = Base + Pos;

        if (node->FwdLink != Header)
        {
            Next = node->FwdLink;
            return 1;
        }

        return 0;
    }

    template<typename Functor>
    void ForEach(Functor f)
    {
        if (IsEmpty())
        {
            return;
        }

        UINT32 Cursor = Header;
        do
        {
            NODE* node = Base + Cursor;
            f(Cursor, node);

            Cursor = node->FwdLink;
        } while (Cursor != Header);
    }

    bool IsEmpty()
    {
        return Empty;
    }

    virtual UINT32 GetCount()
    {
        return Count;
    }

private:
    NODE*& Base;
    UINT32& Header;
    bool& Empty;
    UINT32 Count;
};

/**
 * \brief A resource management wrapper
 */
struct FreePoolEmptyHolder1
{
};
struct FreePoolEmptyHolder2
{
};
struct FreePoolEmptyHolder3
{
};
typedef UINT32 FreePoolIndex_Type;

template<class T1 = FreePoolEmptyHolder1, class T2 = FreePoolEmptyHolder2,
        class T3 = FreePoolEmptyHolder3>
struct FreePool
{
    template<class V>
    static void Alloc(V** p, FreePoolIndex_Type count)
    {
        *p = new V[count];
    }

    static void Alloc(FreePoolEmptyHolder1** p, FreePoolIndex_Type count)
    {
        *p = 0;
    }
    static void Alloc(FreePoolEmptyHolder2** p, FreePoolIndex_Type count)
    {
        *p = 0;
    }
    static void Alloc(FreePoolEmptyHolder3** p, FreePoolIndex_Type count)
    {
        *p = 0;
    }

public:
    FreePool(const FreePoolIndex_Type total_count,
            const FreePoolIndex_Type index_base = 0) :
            total_count(total_count), node_base(new Node[total_count]), index_base(
                    index_base), free_list(node_base, header, tail, free_count)
    {
        Alloc(&t1_base, total_count);
        Alloc(&t2_base, total_count);
        Alloc(&t3_base, total_count);

        free_list.Init(total_count);
    }

    ~FreePool()
    {
        delete [] node_base;

        if (t1_base)
            delete[] t1_base;

        if (t2_base)
            delete[] t2_base;

        if (t3_base)
            delete[] t3_base;
    }

    FreePoolIndex_Type FreeCount() const
    {
        return free_count;
    }

    FreePoolIndex_Type Alloc(T1** p1 = 0, T2** p2 = 0, T3** p3 = 0)
    {
        assert(free_count > 0);
        FreePoolIndex_Type next_free = free_list.PopHeader();

        if (p1)
        {
            *p1 = t1_base + next_free;
        }
        if (p2)
        {
            *p2 = t2_base + next_free;
        }
        if (p3)
        {
            *p3 = t3_base + next_free;
        }

        return (index_base + next_free);
    }

    void Free(const FreePoolIndex_Type index)
    {
        assert(index < total_count);
        free_list.PushTail(index);
    }

    void Free(const T1* p1)
    {
        FreePoolIndex_Type index = (p1 - t1_base) / sizeof(T1);
        Free(index);
    }

    void Free(const T2* p2)
    {
        FreePoolIndex_Type index = (p2 - t2_base) / sizeof(T2);
        Free(index);
    }

    void Free(const T3* p3)
    {
        FreePoolIndex_Type index = (p3 - t3_base) / sizeof(T3);
        Free(index);
    }

    FreePoolIndex_Type IndexBase()
    {
        return index_base;
    }
    T1* Base(T1** p)
    {
        *p = t1_base;
        return t1_base;
    }
    T2* Base(T2** p)
    {
        *p = t2_base;
        return t2_base;
    }
    T3* Base(T3** p)
    {
        *p = t3_base;
        return t3_base;
    }

    T1* At(const FreePoolIndex_Type index, T1** p)
    {
        *p = (t1_base + index);
        return *p;
    }
    T2* At(const FreePoolIndex_Type index, T2** p)
    {
        *p = t2_base + index;
        return *p;
    }
    T3* At(const FreePoolIndex_Type index, T3** p)
    {
        *p = t3_base + index;
        return *p;
    }

public:
    struct Node
    {
        FreePoolIndex_Type NextLink;
    };

    FreePoolIndex_Type total_count;

    Node* node_base;
    T1* t1_base;
    T2* t2_base;
    T3* t3_base;

    FreePoolIndex_Type index_base;
    FreePoolIndex_Type free_count;
    FreePoolIndex_Type header;
    FreePoolIndex_Type tail;

    CountedLinkedList<Node> free_list;
};



struct rlp_tbl{
        UINT64 ctag:10;
        UINT64 ev:1;
        UINT64 s_per_lpg:1;
        UINT64 xfer_typ:2;
        UINT64 rmw_flag:2;
        UINT64 rbn:10;
        UINT64 rlp_state:2;
        UINT64 drop:1;
        UINT64 cancel:1;
        UINT64 last_lp:1;
        UINT64 flba:33;
        unsigned next_ltag:10;
        unsigned nlv:1;
        unsigned prev_ltag:10;
        unsigned plv:1;
        //unsigned nxfr_ltag:10;
        unsigned nxlv:1;
        unsigned lb_cnt:3;
        unsigned rsvd:3;
        unsigned ecc:8;
        unsigned NextLink;
    };

struct free_tbl{
  UINT32 idx;
  unsigned NextLink;
};

struct EPageEntry
    {
        UINT32 NextLink;
        bool LAST;
        UINT16 EPageLength;

        UINT32 FwdLink;
        UINT32 BackLink;

        bool RBLE;
        UINT32 RBLHead;
        UINT32 RBLTail;

        UINT32 UseCount;
        UINT32 BitErrors;

        UINT32 OnesCount;
        UINT8 DA :1;
        UINT8 DD :1;
        UINT8 EC :1;
        UINT8 WB :1;
        UINT8 CE :1;
        UINT8 UC :1;
        UINT8 V :1;

        UINT8 EncKey;
        UINT32 PEC;

        UINT8 ECCParam;
        UINT8 Size;
        UINT32 ECC;
    };

int main(){
  UINT32 tbl_head = 0;
  UINT32 tbl_tail = 0 ;
  UINT32 tbl_count = 4;
  struct rlp_tbl *tbl = new rlp_tbl[tbl_count];

  UINT32 free_tbl_head = 0;
  UINT32 free_tbl_tail = 0 ;
  UINT32 free_tbl_count = 4;
  struct free_tbl *f_tbl = new free_tbl[free_tbl_count];

  CountedLinkedList<rlp_tbl, 1> foo(tbl, tbl_head, tbl_tail, tbl_count); // <- ok
  CountedLinkedList<free_tbl, 1> free_foo(f_tbl, free_tbl_head, free_tbl_tail, free_tbl_count);

#if 1
  foo.Init(tbl_count);
  free_foo.Init(free_tbl_count);
#endif

#if 1
  struct rlp_tbl *tbl0 = tbl;
  tbl0->ctag = 1;
  tbl0->lb_cnt = 2;
  tbl0->rbn = 51;
  tbl0->ev = true;

  struct rlp_tbl *tbl1 = tbl + 1;
  tbl1->ctag = 1;
  tbl1->rbn = 52;
  tbl1->lb_cnt = 2;
  tbl1->ev = true;

  struct rlp_tbl *tbl2 = tbl + 2;
  tbl2->ctag = 2;
  tbl2->rbn = 53;
  tbl2->lb_cnt = 2;
  tbl2->ev = true;

  struct rlp_tbl *tbl3 = tbl + 3;
  tbl3->ctag = 3;
  tbl3->rbn = 54;
  tbl3->lb_cnt = 2;
  tbl3->ev = true;
#endif

//  if(foo.IsEmpty())
//    cout<<"list is empty"<<endl;
//  else
//    cout<<"list is not empty"<<endl;
//
//  //cout<<foo.Count<<endl;
//  //cout<<foo.Base->ctag<<endl;
//
//#if WORKS_BUT_I_DONT_THINK_IS_THE_CORRECT_WAY
//  foo.Base->NextLink = tbl1;
//  tbl1->NextLink = tbl2;
//  tbl2->NextLink = tbl3;
//#endif

#if 0
  UINT32 idx = foo.PopHeader();
  rlp_tbl *rm_tbl = tbl + idx;

  rm_tbl->ctag = 1;

  foo.PushTail(idx);
#endif

#if 0
  while(foo.Base){
    cout<<foo.Base->ctag<<endl;
    foo.Base = foo.Base->NextLink;
  }
#endif
  UINT32 idx;
  UINT32 free_idx;

  //removing entry with ctag=1 and add to free list.
  for(UINT32 i = 0; i< 3; i++){
    idx = foo.PopHeader();
    rlp_tbl *rm_tbl = tbl + idx;

    if(rm_tbl->ctag==1){
      rm_tbl->ev=false;
      cout<<"invalidating entry:"<<idx<<endl;
#if 1
      free_idx = free_foo.PopHeader();
      struct free_tbl *f_e = f_tbl + free_idx;
      f_e->idx = idx;
      free_foo.PushTail(free_idx);
#endif
    }else{
      foo.PushTail(idx);
    }
  }

#if 1
  do{
    free_foo.
    free_idx = free_foo.PopHeader();
    cout<<"free idx:"<<idx<<endl;
    free_foo.PushTail(free_idx);
  }while(free_idx>=0 && free_idx<3);
#endif


  return 0;
}
