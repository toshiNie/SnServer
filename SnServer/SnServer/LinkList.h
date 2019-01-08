#pragma once

class LinkNode
{
public:
	LinkNode() :next(this), prev(this) 
	{

	}
	void insertNext(LinkNode* node)
	{
		node->next = this->next;
		node->prev = this;
		this->next->prev = node;
		this->next = node;
	}
	void insertPrev(LinkNode* node)
	{
		node->next = this;
		node->prev = this->prev;
		node->prev->next = node;
		this->prev = node;
	}
	void remove()
	{
		this->next->prev = this->prev;
		this->prev->next = this->next;
	}
	LinkNode * next;
	LinkNode * prev;
private:
};
