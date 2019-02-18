#pragma once
#include"stdafx.h"
#include"Reactor.h"
#include"TimeHandler.h"
#include"RaftProtocol.h"

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
struct NodeInfo
{
	enum NodeFlag
	{
		NodeVotedForMe = 0x01, //该节点给自己投票了
		NodeVoting = 0x02,     //该节点有投票权
		NodeHasSufficientLog = 0x04 // 
	};
	void *udate;
	//对于每一个服务器，需要发送给他的下一个日志条目的索引值（初始化为领导人最后索引值加一）
	int nextIndex;
	//对于每一个服务器，已经复制给他的日志的最高索引值
	int matchIndex;
	//Flag -> NodeFlag
	int flags;
	//机器id
	int id;
};

class RaftServer
{
public:
	enum class State{Follower, Candidate, Leader};

public:
	void periodic(int timeSinceLastPeriod)
	{
		//选举计时器  follower  收到心跳 归0   Leader 发送心跳 归0
		timeoutElapsed_ += timeSinceLastPeriod;
		if (state_ == State::Leader)
		{
			if (requestTimeout_ <= timeoutElapsed_)
			{
				//sendAppender todo
			}
		}
		else if (electionTimeout_ <= timeoutElapsed_)
		{
			if (nodes_.size() > 1)
			{
				//start election
			}
		}
		if (lastAppliedIndex_ < commitIndex_)
		{
			// commit raft log
		}
	}

	void becomeCandidate()
	{
		currentTerm_++;

		for (auto& node : nodes_)
		{
			node.second.flags &= ~ NodeInfo::NodeVotedForMe;
		}

		voteFor_ = selfInfo_.id;

		currentLeader_ = nullptr;

		state_ = State::Candidate;

		timeoutElapsed_ = std::rand() % electionTimeout_;

		for (auto& node : nodes_)
		{
			//send RequestVote
		}
	}

	void onRecvRequestVote(NodeInfo* node, const RequestVote& request)
	{
		//如果自己的term小于对方的term,证明自己过时了(可能是网络耗时较长),更新为对方的term, 变为follower
		if (currentTerm_ < request.term)
		{
			currentTerm_ = request.term;
			state_ = State::Follower;
			return;
		}

		VoteResponse response;
		if (checkShouldGrantVote(request)) //需要投票
		{
			voteFor_ = request.candidateId;
			response.voteGranted = 1;
			currentLeader_ = nullptr;
			timeoutElapsed_ = 0;
		}
		else
		{
			response.voteGranted = 0;
		}

		response.term = currentTerm_;
		//todo send response back 
	}

	bool checkShouldGrantVote(const RequestVote& request)
	{
		//投票请求过时
		if (request.term < currentTerm_)
		{
			return false;
		}
		// 已经投过票了
		if (voteFor_ != -1)
		{
			return false;
		}
		//获取当前的日志index
		if (logEntrys_.empty())
		{
			return true;
		}
		auto& logEntry = logEntrys_[logEntrys_.size() - 1];
		//理想情况 
		if (logEntry.term < request.lastLogTerm)
		{
			return true;
		}
		//如果任期相等但是对方日志更新(自己也是候选者?)
		if (logEntry.term == request.lastLogTerm && logEntrys_.size() - 1  <= request.lastLogIndex)
		{
			return true;
		}
		return false;
	}

	bool onRecvRequestVoteResponse(NodeInfo* node, const VoteResponse& response)
	{
		if (state_ != State::Candidate)
		{
			return false;
		}

		//收到的返回term比自己大, 恢复Follower
		if (response.term > currentTerm_)
		{
			currentTerm_ = response.term;
			state_ = State::Follower;
			return false;
		}

		//过时的response
		else if (response.term < currentTerm_)
		{
			return 0;
		}

		//被投票了
		if (response.voteGranted == 1)
		{
			nodes_[node->id].flags |= NodeInfo::NodeVotedForMe;
			voteNum_++;
			if (voteNum_ > nodes_.size() / 2)
			{
				state_ = State::Leader;
				voteNum_ = 0;
				for (auto & node : nodes_)
				{
					node.second.nextIndex = logEntrys_.size();
					node.second.matchIndex = 0;
					sendAppendEntries(&node.second);
				}
				return true;
			}

		}
		return false;

	}

	bool sendAppendEntries(NodeInfo* node)
	{
		AppendEntries request;
		request.term = currentTerm_;
		request.leaderCommit = commitIndex_;
		request.prevLogIndex = 0;
		request.prevLogTerm = 0;
		request.entriesNum = 0;
		request.entries = NULL;

		int nextIndex = node->nextIndex;
		
		auto& logEntry = logEntrys_[nextIndex];
		
		request.entriesNum = 1;
		//copy to sendBuffer

		if (nextIndex > 1)
		{
			auto& prevLogEntry = logEntrys_[nextIndex - 1];
			request.prevLogIndex = nextIndex - 1;
			request.prevLogTerm = prevLogEntry.term;
		}

		//send to socket todo

	}

	bool onRecvAppendEntries(NodeInfo *node, const AppendEntries& request)
	{
		requestTimeout_ = 0;
		AppendEntriesResponse response;
		response.term = currentTerm_;
		//如果自己是  候选者 并且收到了term和自己相等的请求, 说明有人成了Leader, 变为Follower
		if (state_ == State::Candidate && currentTerm_  <= request.term)
		{
			voteFor_ = -1;
			state_ = State::Follower;
		}
		//term < leader.term
		if (currentTerm_ < request.term)
		{
			currentTerm_ = request.term;
			response.term = currentTerm_;
			state_ = State::Follower;
		}
		//说明现在的Leader 过时了, 需返回一个最新的term
		else if (request.term < currentTerm_)
		{
			goto FailWithCurrentIndex;
		}
		if (request.prevLogIndex > 0)
		{
			if (request.prevLogIndex > logEntrys_.size())
			{
				//说明日志已经落后Leader, 需返回自己当前的index
				goto FailWithCurrentIndex;
			}
			auto& spLogEntry = logEntrys_[request.prevLogIndex];

			if (spLogEntry.term != request.prevLogTerm)
			{
				logEntrys_.resize(request.prevLogIndex);
				response.currentIndex = request.prevLogIndex - 1;
				//
				goto Fail;
			}
		}
		/* 本地的日志比Leader要多。当本地服务器曾经是Leader，收到了很多客户端请求
		* 并还没来得及同步时会出现这种情况。这时本地无条件删除比Leader多的日志 */
		if (request.entriesNum == 0 && request.prevLogIndex > 0 && request.prevLogIndex + 1 < logEntrys_.size())
		{
			logEntrys_.resize(request.prevLogIndex + 1);
		}

		response.currentIndex = request.prevLogIndex;
		int i = 0;
		/* 跳过请求中已经在本地添加过的日志*/
		for (; i < request.entriesNum; ++i)
		{
			 LogEntry* entry = &request.entries[i];
			 int indexEntry = request.prevLogIndex + 1 + i;
			 if (indexEntry > logEntrys_.size())
			 {
				 break;
			 }
			 if (logEntrys_[indexEntry].term != entry->term)
			 {
				 logEntrys_.resize(indexEntry);
				 break;
			 }
		}
		/* 请求中确认的新日志添加到本地 */
		for (; i < request.entriesNum; ++i)
		{
			logEntrys_.push_back(request.entries[i]);
			response.currentIndex = request.prevLogIndex + 1 + i;
		}

		/* 4. 请求中携带了Leader已经提交到状态机的日志索引，本地同样也更新这个索引，将其
		*    设置为本地最大日志索引和leader_commit中的较小者*/
		if (commitIndex_ < request.leaderCommit)
		{
			int lastLogIndex = max(logEntrys_.size() -1, 1);
			commitIndex_ = min(lastLogIndex, request.leaderCommit);
		}

		currentLeader_ = node;
		response.success = 1;
		response.firstIndex = request.prevLogIndex + 1;
		//Send back
		return true;
	FailWithCurrentIndex:
		response.currentIndex = logEntrys_.size() - 1;
	Fail:
		response.success = 0;
		response.firstIndex = 0;
		return -1;
		//Sendback
	}
	bool onRecvAppendEntriesResponse(NodeInfo *node, const AppendEntriesResponse& response)
	{
		//过时的回复
		if (response.currentIndex != 0 && response.currentIndex <= node->matchIndex)
		{
			return true;
		}
		//自己不是Leader
		if (state_ != State::Leader)
		{
			return false;
		}
		//自己是个过时的Leader, 转化为Follower
		if (currentTerm_ < response.term)
		{
			currentTerm_ = response.term;
			state_ = State::Follower;
			return true;
		}
		else if (currentTerm_ > response.term)
		{
			return true;
		}

		if (response.success == 0)
		{
			int nextIndex = node->nextIndex;
			if (logEntrys_.size() < nextIndex)
			{
				node->nextIndex = min(response.currentIndex + 1, logEntrys_.size() -1);
			}
			else
			{
				node->nextIndex = nextIndex - 1;
			}
			//resend AppendEntries todo
		}

		node->nextIndex = response.currentIndex + 1;
		node->matchIndex = response.currentIndex;

		if (node->flags & 0x02 == 0 && votingCfgChangeLogIndex_ == -1
			&& logEntrys_.size() < response.currentIndex + 1
			)
		{

		}

		int votes = 1;
		int point = response.currentIndex;
		int i = 0;
		for(;)
	}


private:
	//服务器最后一次知道的任期号
	int currentTerm_;
	//记录在当前分期内给哪个Candidate投过票
	int voteFor_;

	std::vector<LogEntry> logEntrys_;


	int commitIndex_;
	//最后被应用到状态机的日志条目索引值（初始化为 0，持续递增）
	int lastAppliedIndex_;

	State state_;
	//被投票的数量
	int voteNum_;
	//计时器
	int timeoutElapsed_;
	//其他节点的信息
	//std::vector<NodeInfo> nodes_;
	std::map<int, NodeInfo> nodes_;

	int electionTimeout_;
	int requestTimeout_;

	NodeInfo selfInfo_;
	NodeInfo* currentLeader_;
	/* 该raft实现每次只进行一个服务器的配置更改，该变量记录raft server
	* 是否正在进行配置更改*/
	int votingCfgChangeLogIndex_;



};