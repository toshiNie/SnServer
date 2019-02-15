#pragma once


struct LogEntry
{
	int index;
	int term;
	int type;
	int len;
	void* buffer;

	LogEntry& operator=(const LogEntry& right)
	{
		index = right.index;
		term = right.term;
		type = right.type;
		len = right.len;
		buffer = new char[len];
		memcpy(buffer, right.buffer, len);
	}
	~LogEntry()
	{
		if (buffer)
		{
			delete[] buffer;
		}
	}

};
struct RequestVote
{
	int term;  //当前任期号
	int candidateId; // 竞选者id
	int lastLogIndex; //竞选者本地保存最新日志index
	int lastLogTerm;// 最新日志的任期
};

struct VoteResponse
{
	int term; //node的任期号 Candidate根据投票结果和node的任期号来更新自己的任期号 
	int voteGranted; //投票结果 1 for success
};



struct AppendEntries
{
	//Leader 的 term
	int term; 
	// 最新一条日志的前一条index, 理想情况下, Follower的index == prevLogIndex
	int prevLogIndex;
	//最新日志的前一条日志的任期号term
	int prevLogTerm;
	//leader当前已经确认提交到状态机FSM的日志索引index，这意味着Follower也可以安全地将该索引index以前的日志提交
	int leaderCommit;
	//这条添加日志消息携带的日志条数，该实现中最多只有一条
	int entriesNum;
	LogEntry* entries;
};

struct AppendEntriesRespanse
{
	//当前任期号
	int term;
	//node成功添加日志时返回ture，即prev_log_index和prev_log_term都比对成功。否则返回false
	int success;
	int currentIndex;
	int firstIndex;
};