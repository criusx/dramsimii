#include "fbdChannel.h"

using namespace std;
using namespace DRAMSimII;

fbdChannel::fbdChannel(const Settings& settings, const SystemConfiguration &sysConfig, Statistics &stats):
Channel(settings, sysConfig, stats),
frameQueue(3)
{

}

/// <summary>
/// find the protocol gap between a command and current system state
/// 
/// </summary>
int fbdChannel::minProtocolGap(const Command *this_c) const
{ 
	// TODO: also consider commands issued one cycle ago which are not yet executed
	int min_gap;

	const unsigned this_rank = this_c->getAddress().getRank();
	const Rank &currentRank = rank[this_rank];
	const Bank &currentBank = currentRank.bank[this_c->getAddress().getBank()];

	int t_al = this_c->isPostedCAS() ? timingSpecification.tAL() : 0;

	switch(this_c->getCommandType())
	{
	case ACTIVATE:
		{
			// respect t_rp of same bank
			int tRPGap = (int)(currentBank.getLastPrechargeTime() - time) + timingSpecification.tRP();

			int ras_q_count = currentRank.lastActivateTimes.size();

			// respect tRRD and tRC of all other banks of same rank
			int tRRDGap;

			if (ras_q_count == 0)
			{
				tRRDGap = 0;
			}
			else 
			{
				// read tail end of ras history
				//tick *last_ras_time = currentRank.lastActivateTimes.read(ras_q_count - 1); 
				const tick lastRASTime = currentRank.lastActivateTimes.back();
				// respect the row-to-row activation delay
				tRRDGap = (int)(lastRASTime - time) + timingSpecification.tRRD();				
			}

			// respect the row cycle time limitation
			int tRCGap = (int)(currentBank.getLastRASTime() - time) + timingSpecification.tRC();

			// respect the t_faw value for DDR2 and beyond
			int tFAWGap;

			if (ras_q_count < 4)
			{
				tFAWGap = 0;
			}
			else
			{
				// read head of ras history
				const tick fourth_ras_time = currentRank.lastActivateTimes.back(); 
				tFAWGap = (int)(fourth_ras_time - time) + timingSpecification.tFAW();
			}

			// respect tRFC
			int tRFCGap = (int)(currentRank.getLastRefreshTime() - time) + timingSpecification.tRFC();

			min_gap = max(max(max(tRFCGap,tRCGap) , tRPGap) , max(tRRDGap , tFAWGap));
		}
		break;

	case READ_AND_PRECHARGE:
		// Auto precharge will be issued as part of command,
		// but DRAM devices are intelligent enough to delay the prec command
		// until tRAS timing is met, so no need to check tRAS timing requirement here.

	case READ:
		{
			//respect last ras of same rank
			int t_ras_gap = (int)((currentBank.getLastRASTime() - time) + timingSpecification.tRCD() - t_al);

			// ensure that if no other rank has issued a CAS command that it will treat
			// this as if a CAS command was issued long ago
			//tick other_r_last_cas_time = time - 1000;
			//int other_r_last_cas_length = timingSpecification.tBurst();
			//tick other_r_last_casw_time = time - 1000;
			//int other_r_last_casw_length = timingSpecification.tBurst();

			// find the most recent cas(w) time and length
			//for (unsigned rank_id = 0; rank_id < rank.size() ; rank_id++)
			//{
			//	if (rank_id != this_rank)
			//	{
			//		if (rank[rank_id].lastCASTime > other_r_last_cas_time)
			//		{
			//			other_r_last_cas_time = rank[rank_id].lastCASTime;
			//			other_r_last_cas_length = rank[rank_id].lastCASLength;
			//		}
			//		if (rank[rank_id].lastCASWTime > other_r_last_casw_time)
			//		{
			//			other_r_last_casw_time = rank[rank_id].lastCASWTime;
			//			other_r_last_casw_length = rank[rank_id].lastCASWLength;
			//		}
			//	}
			//}

			//respect last cas of same rank
			// DW 3/9/2006 add these two lines
			//cas_length = max(timing_specification.t_int_burst,this_r.last_cas_length);
			//casw_length = max(timing_specification.t_int_burst,this_r.last_casw_length);
			// DW 3/9/2006 replace the line after next with the next line
			//t_cas_gap = max(0,(int)(this_r.last_cas_time + cas_length - now));
			int t_cas_gap = (int)((currentRank.getLastCASTime() - time) + timingSpecification.tBurst());

			//respect last cas write of same rank
			// DW 3/9/2006 replace the line after next with the next line
			//t_cas_gap = max(t_cas_gap,(int)(this_r.last_casw_time + timing_specification.t_cwd + casw_length + timing_specification.t_wtr - now));
			t_cas_gap = max(t_cas_gap,(int)((currentRank.getLastCASWTime() - time) + timingSpecification.tCWD() + timingSpecification.tBurst() + timingSpecification.tWTR()));

			//if (rank.size() > 1) 
			//{
			//respect most recent cas of different rank
			//	t_cas_gap = max(t_cas_gap,(int)(other_r_last_cas_time + other_r_last_cas_length + timingSpecification.tRTRS() - time));
			//respect timing of READ follow WRITE, different ranks.*/
			//	t_cas_gap = max(t_cas_gap,(int)(other_r_last_casw_time + timingSpecification.tCWD() + other_r_last_casw_length + timingSpecification.tRTRS() - timingSpecification.tCAS() - time));
			//}
			min_gap = max(t_ras_gap,t_cas_gap);

			//fprintf(stderr," [%8d] [%8d] [%8d] [%8d] [%8d] [%2d]\n",(int)now,(int)this_r_last_cas_time,(int)this_r_last_casw_time,(int)other_r_last_cas_time,(int)other_r_last_casw_time,min_gap);
		}
		break;

	case WRITE_AND_PRECHARGE:
		// Auto precharge will be issued as part of command, so
		// Since commodity DRAM devices are write-cycle limited, we don't have to worry if
		// the precharge will met tRAS timing or not. So WRITE_AND_PRECHARGE
		// has the exact same timing requirements as a simple CAS COMMAND.

	case WRITE:
		{
			//respect last ras of same rank
			int t_ras_gap = (int)((currentBank.getLastRASTime() - time) + timingSpecification.tRCD() - t_al);

			/*tick other_r_last_cas_time = time - 1000;
			int other_r_last_cas_length = timingSpecification.tBurst();
			tick other_r_last_casw_time = time - 1000;
			int other_r_last_casw_length = timingSpecification.tBurst();*/

			// find the most recent CAS/CASW time and length
			// FIXME: change to use iterators
			/*for (unsigned rank_id = 0; rank_id < rank.size() ; rank_id++)
			{
			if (rank_id != this_rank)
			{
			if (rank[rank_id].lastCASTime > other_r_last_cas_time)
			{
			other_r_last_cas_time = rank[rank_id].lastCASTime;
			other_r_last_cas_length = rank[rank_id].lastCASLength;
			}
			if (rank[rank_id].lastCASWTime > other_r_last_casw_time)
			{
			other_r_last_casw_time = rank[rank_id].lastCASWTime;
			other_r_last_casw_length = rank[rank_id].lastCASWLength;
			}
			}
			}*/

			// respect last cas to same rank
			int t_cas_gap = (int)(currentRank.getLastCASTime() - time) + timingSpecification.tCAS() + timingSpecification.tBurst() + timingSpecification.tRTRS() - timingSpecification.tCWD();

			// respect last cas to different ranks
			// not in FBD, each rank is on its own channel
			//t_cas_gap = max(t_cas_gap,(int)(other_r_last_cas_time + timingSpecification.tCAS() + other_r_last_cas_length + timingSpecification.tRTRS() - timingSpecification.tCWD() - time));

			// respect last cas write to same rank
			t_cas_gap = max(t_cas_gap,(int)(currentRank.getLastCASWTime() - time) + timingSpecification.tBurst());

			// respect last cas write to different ranks
			// not in FBD
			//t_cas_gap = max(t_cas_gap,(int)(other_r_last_casw_time + other_r_last_casw_length - time));

			min_gap = max(t_ras_gap,t_cas_gap);
		}
		break;

	case RETIRE_COMMAND:
		break;

	case PRECHARGE:
		{
			// respect t_ras of same bank
			int t_ras_gap = (int)(currentBank.getLastRASTime() - time) + timingSpecification.tRAS();

			// respect t_cas of same bank
			int t_cas_gap = max(0,(int)((currentBank.getLastCASTime() - time) + t_al + timingSpecification.tCAS() + timingSpecification.tBurst() + max(0,timingSpecification.tRTP() - timingSpecification.tCMD())));

			// respect t_casw of same bank
			t_cas_gap = max(t_cas_gap,(int)((currentBank.getLastCASWTime() - time) + t_al + timingSpecification.tCWD() + timingSpecification.tBurst() + timingSpecification.tWR()));

			min_gap = max(t_ras_gap,t_cas_gap);
		}
		break;

	case PRECHARGE_ALL:
		break;

	case ACTIVATE_ALL:
		break;

	case DRIVE_COMMAND:
		break;

	case DATA_COMMAND:
		break;

	case CAS_WITH_DRIVE_COMMAND:
		break;

	case REFRESH_ALL:
		// respect tRFC and tRP
		min_gap = max((int)((currentRank.getLastRefreshTime() - time) + timingSpecification.tRFC()),(int)((currentRank.getLastPrechargeTime() - time) + timingSpecification.tRP()));
		break;

	default:
		exit(-20);
		break;
	}

#ifdef DEBUG_MIN_PROTOCOL_GAP
	outStream << this_c->commandType;
	outStream << " ras[" << setw(2) << t_ras_gap << "] rrd[" << setw(2) << t_rrd_gap << "] faw[" << setw(2) << t_faw_gap << "] cas[" << setw(2) << t_cas_gap << "] rrd[" << setw(2) << t_rrd_gap << "] rp[" << setw(2) << t_rp_gap << "] min[" << setw(2) << min_gap << "]" << endl;
#endif

	//return max(min_gap,timingSpecification.tCMD());
	return min_gap;
}


// may only execute commands from the previous frame or from the first slot in the next frame
// however, up to three commands must be executed
const void *fbdChannel::moveChannelToTime(const tick endTime, tick& transFinishTime)
{

	while (time < endTime)
	{
		// try to convert any transactions into commands first
		if (checkForAvailableCommandSlots(readTransaction(true)))
		{
			// actually remove it from the queue now
			Transaction *decodedTransaction = getTransaction();

			// then break into commands and insert into per bank command queues
			bool t2cResult = transaction2commands(decodedTransaction);
			assert(t2cResult == true);

			DEBUG_TRANSACTION_LOG("T->C [" << time << "] Q[" << getTransactionQueueCount() << "]" << decodedTransaction);
		}
		// if all available transactions are converted to commands, then execute the frame corresponding to this time
		else
		{
			// move time up by executing frames
			fbdFrame *nextFrame = getNextFrame();

			// if there's a frame at this time
			if (nextFrame)
			{
				// execute any commands in this frame
				if (nextFrame->getCommandAType() != INVALID_COMMAND)
				{
					statistics.collectCommandStats(nextFrame->getCommandA());
					DEBUG_COMMAND_LOG("C F[" << std::hex << setw(8) << time << "] MG[" << setw(2) << 0 << "] " << *nextFrame->getCommandA());
					executeCommand(nextFrame->getCommandA());
				}
				if (nextFrame->getCommandBType() != INVALID_COMMAND && nextFrame->getCommandBType() != DATA_COMMAND)
				{
					statistics.collectCommandStats(nextFrame->getCommandB());
					DEBUG_COMMAND_LOG("C F[" << std::hex << setw(8) << time << "] MG[" << setw(2) << 0 << "] " << *nextFrame->getCommandB());
					executeCommand(nextFrame->getCommandB());
				}
				if (nextFrame->getCommandCType() != INVALID_COMMAND && nextFrame->getCommandCType() != DATA_COMMAND)
				{
					statistics.collectCommandStats(nextFrame->getCommandC());
					DEBUG_COMMAND_LOG("C F[" << std::hex << setw(8) << time << "] MG[" << setw(2) << 0 << "] " << *nextFrame->getCommandC());
					executeCommand(nextFrame->getCommandC());
				}
			}

			delete nextFrame;

			// then build a frame for t+1, if possible
			makeFrame(time);
			
			// only get completed commands if they have finished TODO:
			if (Transaction *completed_t = completionQueue.pop())
			{
				statistics.collectTransactionStats(completed_t);

				DEBUG_TRANSACTION_LOG("T CH[" << setw(2) << channelID << "] " << completed_t);

				// reuse the refresh transactions
				if (completed_t->getType() == AUTO_REFRESH_TRANSACTION)
				{
					completed_t->setEnqueueTime(completed_t->getEnqueueTime() + systemConfig.getRefreshTime());

					assert(systemConfig.getRefreshPolicy() != NO_REFRESH);

					delete completed_t;

					return NULL;
				}
				else // return what was pointed to
				{
					const void *origTrans = completed_t->getOriginalTransaction();

					transFinishTime = completed_t->getCompletionTime();

					M5_DEBUG(assert(completed_t->getOriginalTransaction()));								

					delete completed_t;

					return origTrans;
				}
			}

			// last, move time forward to either the next transaction decode or frame create time
			tick nextDecodeTime = nextTransactionDecodeTime();
			assert(nextDecodeTime > time);
			tick nextFrameTime = nextFrameExecuteTime();
			assert(nextFrameTime > time);

			
			time = min(endTime, min(nextDecodeTime, nextFrameTime));
		}		
	}


	assert(time <= endTime + timingSpecification.tCMD());

	//*transFinishTime = endTime;

	M5_TIMING_LOG("ch[" << channelID << "] @ " << std::dec << time);

	return NULL;

}

tick fbdChannel::nextFrameExecuteTime() const
{
	assert(frameQueue.front()->getExecuteTime() > time);
	return frameQueue.front()->getExecuteTime();
}

fbdFrame *fbdChannel::getNextFrame() 
{
	assert(frameQueue.front()->getExecuteTime() == time);
	return frameQueue.pop();
}

//////////////////////////////////////////////////////////////////////////
// attempts to create either a CMD or CMD+D frame or append an additional
// CMD to an existing CMD+D frame
//////////////////////////////////////////////////////////////////////////
bool fbdChannel::makeFrame(const tick currentTime)
{
	assert(frameQueue.back()->getExecuteTime() != currentTime);

	// decide if a new frame should be made or if commands can be added to an existing CMD+D frame
	fbdFrame *newFrame;

	// then there is already a frame here that contains data so try to find a command to fill the A slot
	if (frameQueue.front()->getExecuteTime() == currentTime)
	{
		newFrame = frameQueue.read(0);
		// search for an A command, something that can be executed in < 2 ticks
		// this command should also not conflict with the others in the queue
		// or others
		const Command *newCommand = readNextCommand(NULL, NULL);
		// if the command can be issued on the next tick
		if (newCommand && (minProtocolGap(newCommand) - timingSpecification.tCMD() < 1))
		{
			newFrame->setCommandA(getNextCommand(NULL, NULL));
		}
	}
	else
	{
		newFrame = new fbdFrame(currentTime);
		frameQueue.push(newFrame);

		// search for an A command, something that can be executed in < 2 ticks
		// this command should also not conflict with the others in the queue
		// or others
		const Command *newCommand = readNextCommand(NULL, NULL);
		// if the command can be issued on the next tick
		if (newCommand && (minProtocolGap(newCommand) - timingSpecification.tCMD() < 1))
		{
			newFrame->setCommandA(getNextCommand(NULL, NULL));

			// if this was a CAS+W then the frame type will be CMD+D
			if (newCommand->getCommandType() == WRITE_AND_PRECHARGE || newCommand->getCommandType() == WRITE)
			{
				newFrame->setCommandB(NULL);
				newFrame->setCommandC(NULL);
				// create some new frames for the remainder of the data
				for (int i = 1; i < timingSpecification.tBurst(); ++i)
				{
					fbdFrame *extraFrame = new fbdFrame(currentTime + i);
					extraFrame->setCommandA(NULL);
					extraFrame->setCommandB(NULL);
					extraFrame->setCommandC(NULL);
					frameQueue.push(extraFrame);
				}
			}
			// otherwise try to find two more commands to fill the B and C slots
			else
			{
				// overloaded readNextCommand() will only return commands which do not interfere with the commands passed in
				if (readNextCommand(newCommand, NULL))
				{
					newFrame->setCommandB(getNextCommand(newCommand, NULL));
				}
				if (readNextCommand(newCommand, newFrame->getCommandB()))
				{
					newFrame->setCommandC(getNextCommand(newCommand, newFrame->getCommandB()));
				}
			}
		}
	}

	return true;
}


/// <summary>
/// Chooses the command which should be executed next from the given channel
/// Choice is made based on command_ordering_algorithm from system config
/// Command returned has already been removed from the per_bank_command_queue
/// from which it was selected
/// </summary>
Command *fbdChannel::getNextCommand(const Command *slotACommand, const Command *slotBCommand)
{
	const Command *nextCommand = readNextCommand(slotACommand, slotBCommand);

	if (nextCommand)
	{
		Rank &currentRank = rank[nextCommand->getAddress().getRank()];

		// if it was a refresh all command, then dequeue all n banks worth of commands
		if (nextCommand->getCommandType() == REFRESH_ALL)
		{
			Command *tempCommand = NULL;

			for (vector<Bank>::iterator cur_bank = currentRank.bank.begin(); cur_bank != currentRank.bank.end(); cur_bank++)
			{
				if (tempCommand)
					delete tempCommand;
				tempCommand = cur_bank->pop();
				assert(tempCommand->getCommandType() == REFRESH_ALL);
			}

			return tempCommand;

		}
		else
		{
			//bank_c &currentBank = currentRank.bank[nextCommand->getAddress().bank_id];
			assert(currentRank.bank[nextCommand->getAddress().getBank()].front() == nextCommand);
			return currentRank.bank[nextCommand->getAddress().getBank()].pop();
		}
	}
	else
	{
		return NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
// executes the commands in the A slot from one cycle ago or the B/C slot
// from two cycles ago
//////////////////////////////////////////////////////////////////////////
bool fbdChannel::executeFrame(const tick currentTime)
{
	return true;
}

/// <summary>
/// Chooses the command which should be executed next from the given channel
/// Choice is made based on command_ordering_algorithm from system config
/// Command returned is not yet removed from the per_bank_command_queue
/// from which it was selected
/// </summary>
// TODO: do not select commands which go to the same chan/rank/bank as the slotACommand
const Command *fbdChannel::readNextCommand(const Command *slotACommand, const Command *slotBCommand) const
{
	// look at the most recently retired command in this channel's history

	const Command *lastCommand = historyQueue.back();

	unsigned lastBankID = lastCommand ? lastCommand->getAddress().getBank() : systemConfig.getBankCount() - 1;
	unsigned lastRankID = lastCommand ? lastCommand->getAddress().getRank() : systemConfig.getRankCount() - 1;
	const CommandType lastCommandType = lastCommand ? lastCommand->getCommandType() : WRITE_AND_PRECHARGE;

	const int slotARank = slotACommand ? slotACommand->getAddress().getRank() : -1;
	const int slotBRank = slotBCommand ? slotBCommand->getAddress().getRank() : -1;

	switch (systemConfig.getCommandOrderingAlgorithm())
	{
		// this strategy attempts to find the oldest command and returns that to be executed
		// however, if the oldest command cannot be issued, the oldest command that can be executed immediately
		// will be returned instead
	case STRICT_ORDER:
		{
			tick oldestCommandTime = TICK_MAX;
			tick oldestExecutableCommandTime = TICK_MAX;
			vector<Bank>::const_iterator oldestBank;
			vector<Bank>::const_iterator oldestExecutableBank;

			for (vector<Rank>::const_iterator currentRank = rank.begin(); currentRank != rank.end(); currentRank++)
			{
				// do not consider ranks which have already had a command chosen from them
				if (currentRank->getRankID() == slotARank || currentRank->getRankID() == slotBRank)
					continue;

				bool notAllRefresh = false;

				for (vector<Bank>::const_iterator bank_id = currentRank->bank.begin(); bank_id != currentRank->bank.end(); bank_id++)
				{
					if (const Command *temp_c = bank_id->front())
					{
						if ((temp_c->getEnqueueTime() < oldestExecutableCommandTime) && (minProtocolGap(temp_c) <= timingSpecification.tCMD()))
						{
							// if it's a refresh_all command and
							// we haven't proved that all the queues aren't refresh_all commands, search
							if (temp_c->getCommandType() == REFRESH_ALL)
							{
								if (!notAllRefresh)
								{
									// try to show that at the head of each queue isn't a refresh command
									for (vector<Bank>::const_iterator currentBank = currentRank->bank.begin(); currentBank != currentRank->bank.end(); currentBank++)
									{
										// if any queue is empty or the head of any queue isn't a refresh command, mark this fact and do not choose refresh
										if ((currentBank->size() == 0) || ((currentBank->front()) && (currentBank->front()->getCommandType() != REFRESH_ALL)))
										{
											notAllRefresh = true;
											break;
										}

									}
									// if all are known now to be refresh commands
									if (!notAllRefresh)
									{
										oldestExecutableCommandTime = temp_c->getEnqueueTime();
										oldestExecutableBank = bank_id;
									}
								}
							}
							else if (temp_c->getEnqueueTime() < oldestCommandTime)
							{
								// if it's a refresh_all command and
								// we haven't proved that all the queues aren't refresh_all commands, search
								if (temp_c->getCommandType() == REFRESH_ALL)
								{
									if (!notAllRefresh)
									{
										// try to show that at the head of each queue isn't a refresh command
										for (vector<Bank>::const_iterator currentBank = currentRank->bank.begin(); currentBank != currentRank->bank.end(); currentBank++)
										{
											// if any queue is empty or the head of any queue isn't a refresh command, mark this fact and do not choose refresh
											if ((currentBank->size() == 0) || ((currentBank->front()) && (currentBank->front()->getCommandType() != REFRESH_ALL)))
											{
												notAllRefresh = true;
												break;
											}

										}
										// if all are known now to be refresh commands
										if (!notAllRefresh)
										{
											oldestCommandTime = temp_c->getEnqueueTime();
											oldestBank = bank_id;
										}
									}
								}
								else
								{
									oldestCommandTime = temp_c->getEnqueueTime();
									oldestBank = bank_id;
								}

							}

							else
							{
								oldestExecutableCommandTime = temp_c->getEnqueueTime();
								oldestExecutableBank = bank_id;
							}
						}
					}
				}
			}

			// if any executable command was found, prioritize it over those which must wait
			if (oldestExecutableCommandTime < TICK_MAX)
			{
				assert(oldestExecutableBank->nextCommandType() == REFRESH_ALL || rank[oldestExecutableBank->front()->getAddress().getRank()].bank[oldestExecutableBank->front()->getAddress().getBank()].front() == oldestExecutableBank->front());

				return oldestExecutableBank->front();
			}
			// if there was a command found
			else if (oldestCommandTime < TICK_MAX)
			{
				assert(oldestBank->front()->getCommandType() == REFRESH_ALL || rank[oldestBank->front()->getAddress().getRank()].bank[oldestBank->front()->getAddress().getBank()].front() == oldestBank->front());

				return oldestBank->front();
			}
			else
				return NULL;
		}
		break;

		// alternate ranks as we go down banks
	case RANK_ROUND_ROBIN:
		{
			TransactionType transactionType;

			switch (lastCommandType)
			{
				// if it was RAS before and you want to finish doing the read/write
			case ACTIVATE:
				{
					const Command *temp_c =  rank[lastRankID].bank[lastBankID].front();

					if ((temp_c) &&
						((temp_c->getCommandType() == WRITE_AND_PRECHARGE) ||
						(temp_c->getCommandType() == READ_AND_PRECHARGE) ||
						(temp_c->getCommandType() == READ) ||
						(temp_c->getCommandType() == WRITE)))
					{
						return rank[lastRankID].bank[lastBankID].front();
					}
					else
					{
						cerr << "Serious problems. RAS not followed by CAS" << endl;
						exit(2);
					}
				}
				break;

			case READ_AND_PRECHARGE:
			case READ:
			case PRECHARGE:
				transactionType = READ_TRANSACTION;
				break;

			case REFRESH_ALL:
			case WRITE:
			case WRITE_AND_PRECHARGE:
				transactionType = WRITE_TRANSACTION;
				break;	

			default:
				transactionType = READ_TRANSACTION;
				cerr << "Did not find a CAS or RAS command when it was expected" << endl;
				break;
			}

			unsigned originalLastRankID = lastRankID;
			unsigned originalLastBankID = lastBankID;
			TransactionType originalTransactionType = transactionType;
			bool noPendingRefreshes = false;

			while (true)
			{
				// select the next rank
				lastRankID = (lastRankID + 1) % systemConfig.getRankCount();

				// select the next bank when all ranks at this bank have been checked
				if (lastRankID == originalLastRankID)
				{
					if (!noPendingRefreshes)
					{
						// before switching to the next bank, see if all the queues are refreshes in any rank
						for (vector<Rank>::const_iterator currentRank = rank.begin(); currentRank != rank.end(); currentRank++)
						{
							bool notAllRefresh = false;
							for (vector<Bank>::const_iterator currentBank = currentRank->bank.begin(); currentBank != currentRank->bank.end(); currentBank++)
							{
								// if any queue is empty or the head of any queue isn't a refresh command, mark this fact and do not choose refresh
								if ((currentBank->size() == 0) || ((currentBank->front()) && (currentBank->front()->getCommandType() != REFRESH_ALL)))
								{
									notAllRefresh = true;
									break;
								}
							}
							// are all the commands refreshes? if so then return this
							if (!notAllRefresh)
								return currentRank->bank[lastBankID].front(); // which bank doesn't really matter
						}
						noPendingRefreshes = true;
					}

					lastBankID = (lastBankID + 1) % systemConfig.getBankCount();

					// when all ranks and all banks have been checked for a read/write, look for a write/read
					if (lastBankID == originalLastBankID)
					{

						transactionType = (transactionType == WRITE_TRANSACTION) ? READ_TRANSACTION : WRITE_TRANSACTION;
						// however, if this type has already been searched for, then there are no commands, so quit
						if (transactionType == originalTransactionType)
							break;
					}
				}

				const Command *temp_c = rank[lastRankID].bank[lastBankID].front();

				if (temp_c && temp_c->getCommandType() != REFRESH_ALL)
				{
					if (!systemConfig.isReadWriteGrouping())
					{
						return temp_c;
					}
					else // have to follow read_write grouping considerations 
					{
						// look at the second command
						const Command *next_c = rank[lastRankID].bank[lastBankID].read(1);	

						if (next_c &&
							((next_c->getCommandType() == READ_AND_PRECHARGE || next_c->getCommandType() == READ) && (transactionType == READ_TRANSACTION)) ||
							((next_c->getCommandType() == WRITE_AND_PRECHARGE || next_c->getCommandType() == WRITE) && (transactionType == WRITE_TRANSACTION)))
						{
							assert(rank[lastRankID].bank[lastBankID].front()->getAddress().getBank() == lastBankID);
							assert(rank[lastRankID].bank[lastBankID].front()->getAddress().getRank() == lastRankID);
							return rank[lastRankID].bank[lastBankID].front();
						}
					}

#ifdef DEBUG_FLAG_2
					cerr << "Looked in ["<< temp_c->getAddress().rank << "] [" << temp_c->getAddress().bank << "] but wrong type, We want [" << transactionType << "]. Candidate command type ";
					cerr << temp_c->getCommandType();
					cerr << " followed by ";
					cerr << rank[lastRankID].bank[lastBankID].getPerBankQueue().read(1)->getCommandType();
					cerr << "count [" << rank[lastRankID].bank[lastBankID].getPerBankQueue().get_count() << "]" << endl;
#endif

				}

#ifdef DEBUG_FLAG_2
				cerr << "Looked in [" << lastRankID << "] [" << lastBankID << "] but Q empty" << endl;
#endif

			}
		}
		break;

		// keep rank id as long as possible, go round robin down a given rank
	case BANK_ROUND_ROBIN: 
		{			
			TransactionType transactionType;

			switch (lastCommandType)
			{
			case ACTIVATE:
				{
					const Command *temp_c = rank[lastRankID].bank[lastBankID].front();

					if ((temp_c) &&
						((temp_c->getCommandType() == WRITE_AND_PRECHARGE) ||
						(temp_c->getCommandType() == READ_AND_PRECHARGE) ||
						(temp_c->getCommandType() == READ) ||
						(temp_c->getCommandType() == WRITE)))
					{
						return rank[lastRankID].bank[lastBankID].front();
					}
					else
					{
						cerr << "Serious problems. RAS not followed by CAS." << endl;
						exit(2);
					}
				}
				break;
			case READ_AND_PRECHARGE:
			case READ:
			case PRECHARGE:
				transactionType = READ_TRANSACTION;
				break;

			case REFRESH_ALL:
			case WRITE:
			case WRITE_AND_PRECHARGE:
				transactionType = WRITE_TRANSACTION;
				break;
			default:
				transactionType = WRITE_TRANSACTION; // FIXME: added this to ensure no uninit vars
				cerr << "Did not find a CAS or RAS command when it was expected" << endl;
				break;
			}

			unsigned originalLastRankID = lastRankID;
			unsigned originalLastBankID = lastBankID;
			TransactionType originalTransactionType = transactionType;
			bool noPendingRefreshes = false;

			while (true)
			{
				// select the next bank
				lastBankID = (lastBankID + 1) % systemConfig.getBankCount();

				if (lastBankID == originalLastBankID)
				{
					if (!noPendingRefreshes)
					{
						// before switching to the next bank, see if all the queues are refreshes in any rank
						for (vector<Rank>::const_iterator currentRank = rank.begin(); currentRank != rank.end(); currentRank++)
						{
							bool notAllRefresh = false;
							for (vector<Bank>::const_iterator currentBank = currentRank->bank.begin(); currentBank != currentRank->bank.end(); currentBank++)
							{
								// if any queue is empty or the head of any queue isn't a refresh command, mark this fact and do not choose refresh
								if ((currentBank->size() == 0) || ((currentBank->front()) && (currentBank->front()->getCommandType() != REFRESH_ALL)))
								{
									notAllRefresh = true;
									break;
								}
							}
							// are all the commands refreshes? if so then return this
							if (!notAllRefresh)
								return currentRank->bank[lastBankID].front(); // which bank doesn't really matter
						}
						noPendingRefreshes = true;
					}

					lastRankID = (lastRankID + 1) % systemConfig.getRankCount();

					if (lastRankID == originalLastRankID)
					{
						transactionType = (transactionType == WRITE_TRANSACTION) ? READ_TRANSACTION : WRITE_TRANSACTION;
						// however, if this type has already been searched for, then there are no commands, so quit
						if (transactionType == originalTransactionType)
							break;
					}
				}

				const Command *temp_c = rank[lastRankID].bank[lastBankID].front();

				if (temp_c && temp_c->getCommandType() != REFRESH_ALL)
				{	
					if(systemConfig.isReadWriteGrouping() == false)
					{
						return temp_c;
					}
					else // have to follow read_write grouping considerations
					{
						// look at the second command
						const Command *next_c =  rank[lastRankID].bank[lastBankID].read(1);

						if (next_c &&
							((next_c->getCommandType() == READ_AND_PRECHARGE || next_c->getCommandType() == READ) && (transactionType == READ_TRANSACTION)) ||
							((next_c->getCommandType() == WRITE_AND_PRECHARGE || next_c->getCommandType() == WRITE) && (transactionType == WRITE_TRANSACTION)))
						{
							assert(rank[lastRankID].bank[lastBankID].front()->getAddress().getBank() == lastBankID);
							assert(rank[lastRankID].bank[lastBankID].front()->getAddress().getRank() == lastRankID);
							return rank[lastRankID].bank[lastBankID].front();
						}
					}

#ifdef DEBUG_FLAG_2
					cerr << "Looked in ["<< temp_c->getAddress().rank << "] [" << temp_c->getAddress().bank << "] but wrong type, We want [" << transactionType << "]. Candidate command type ";
					cerr << temp_c->getCommandType();
					cerr << " followed by ";
					cerr << rank[lastRankID].bank[lastBankID].getPerBankQueue().read(1)->getCommandType();
					cerr << "count [" << rank[lastRankID].bank[lastBankID].getPerBankQueue().get_count() << "]" << endl;
#endif

				}

#ifdef DEBUG_FLAG_2
				cerr << "Looked in rank=[" << lastRankID << "] bank=[" << lastBankID << "] but Q empty" << endl;
#endif

			}
		}
		break;

	case GREEDY:
		{
			const Command *candidateCommand = NULL;

			int candidateGap = INT_MAX;

			for (vector<Rank>::const_iterator currentRank = rank.begin(); currentRank != rank.end(); currentRank++)
			{
				bool notAllRefresh = false;

				for (vector<Bank>::const_iterator currentBank = currentRank->bank.begin(); currentBank != currentRank->bank.end(); currentBank++)
				{
					const Command *challengerCommand = currentBank->front();

					if (challengerCommand)
					{
						int challengerGap = minProtocolGap(challengerCommand);

						if (challengerGap < candidateGap || (candidateGap == challengerGap && challengerCommand->getEnqueueTime() < candidateCommand->getEnqueueTime()))
						{
							if (challengerCommand->getCommandType() == REFRESH_ALL)
							{					
								// if it hasn't been proven to be all refreshes or not yet
								if (!notAllRefresh)
								{
									for (vector<Bank>::const_iterator thisBank = currentRank->bank.begin(); thisBank != currentRank->bank.end(); thisBank++)
									{
										// if any queue is empty or the head of any queue isn't a refresh command, mark this fact and do not choose refresh
										if ((thisBank->size() == 0) || (thisBank->front()->getCommandType() != REFRESH_ALL))
										{
											notAllRefresh = true;
											break;
										}
									}
									// are all the commands refreshes? if so then choose this
									if (!notAllRefresh)
									{
										candidateGap = challengerGap;
										candidateCommand = challengerCommand;
									}
								}
							}
							else
							{
								candidateGap = challengerGap;
								candidateCommand = challengerCommand;
							}							
						}
					}
				}
			}

			if (candidateCommand)
			{
				assert(candidateCommand->getCommandType() == REFRESH_ALL || rank[candidateCommand->getAddress().getRank()].bank[candidateCommand->getAddress().getBank()].front() == candidateCommand);

				DEBUG_TIMING_LOG("R[" << candidateCommand->getAddress().getRank() << "] B[" << candidateCommand->getAddress().getBank() << "]\tWinner: " << *candidateCommand << "gap[" << candidateGap << "] now[" << time << "]")
			}

			return candidateCommand;
		}
		break;

	default:
		{
			cerr << "This configuration and algorithm combination is not supported" << endl;
			exit(0);
		}
		break;
	}
	return NULL;
}
