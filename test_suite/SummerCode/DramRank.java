import java.util.Stack;


public class DramRank {
	private Stack<DramCommand> recentCommands;
	private DramBank[] banks;
	
	public DramRank(int numBanks){
		recentCommands = new Stack<DramCommand>();
		recentCommands.setSize(5);
		banks = new DramBank[numBanks];
		for(int i = 0; i < numBanks ; i++){
			banks[i] = new DramBank();
		}		
	}
	
	public String recieveCommand(DramCommand com){
		recentCommands.push(com);
		recentCommands.setSize(5); //To make sure no more than 5 commands are stored at a time
		return checkTiming(com) + sendCommand(com);
	}
	
	public String sendCommand(DramCommand com){
		return banks[com.getBank()].recieveCommand(com);
	}
	
	public String checkTiming(DramCommand com){
		if(com.getType()== DramCommand.CommandType.ACTIVATE){
			if(recentCommands.get(1).getType()== DramCommand.CommandType.ACTIVATE){
				
			}
		}
		return "";
	}
}
