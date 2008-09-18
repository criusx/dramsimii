
public class DramCommand {
	
	private String command;
	private String F,type,addr,col,row;
	private int MG,rank,bank,S,Q,E,T,DLY,chan;
	
	//This constructor takes a CAS+P command and returns only the CAS part or the Pre part depending on type
	//if type = 0 return CAS else return Pre
	private DramCommand(DramCommand casp, int type){
		switch(type){
		case 0: //Return CAS Assumes end time is end of data burst
			F = casp.getF();
			MG = casp.getMG();
			this.type = "CAS";
			addr = casp.getAddr();
			chan = casp.getChan();
			rank = casp.getRank();
			bank = casp.getBank();
			row  = casp.getRow();
			col  = casp.getCol();
			S = casp.getS()+DramSimValid.getTimingParameter("tAL");
			Q = casp.getQ();
			E = DramSimValid.getTimingParameter("tCAS") + DramSimValid.getTimingParameter("tBurst") + S;
			T = E-S;
			DLY = S-Q;			
			break;
		default: //return Pre Assume end of Pre is at end of array precharge tRP
			F = casp.getF();
			MG = casp.getMG();
			this.type = "Pre";
			addr = casp.getAddr();
			chan = casp.getChan();
			rank = casp.getRank();
			bank = casp.getBank();
			row  = casp.getRow();
			col  = casp.getCol();
			int minimumTimeToWait = DramSimValid.getTimingParameter("tAL") + DramSimValid.getTimingParameter("tBurst") +
									DramSimValid.getTimingParameter("tRTP") - DramSimValid.getTimingParameter("tCCD");
			//Assumes Pre starts after the minimum time to wait after the CAS starts
			S = casp.getS()+ minimumTimeToWait;
			Q = casp.getQ();
			//Assumes Pre ends after tRP
			E = S + DramSimValid.getTimingParameter("tRP");
			T = E-S;
			DLY = S-Q;
		}
		
	}
	
	public DramCommand(String l){
		l = l.substring(2);
		command = l;
		F = l.substring(l.indexOf('[')+1, l.indexOf(']')).trim();
		l = l.substring(l.indexOf(']')+2);
		MG = Integer.parseInt(l.substring(l.indexOf('[')+1, l.indexOf(']')).trim());
		l = l.substring(l.indexOf(']')+2);
		type = l.substring(0, l.indexOf("addr")).trim();
		l = l.substring(type.length());
		addr = l.substring(l.indexOf('[')+1, l.indexOf(']')).trim();
		l = l.substring(l.indexOf(']')+2);
		chan = Integer.parseInt(l.substring(l.indexOf('[')+1, l.indexOf(']')).trim());
		l = l.substring(l.indexOf(']')+2);
		rank = Integer.parseInt(l.substring(l.indexOf('[')+1, l.indexOf(']')).trim());
		l = l.substring(l.indexOf(']')+2);
		bank = Integer.parseInt(l.substring(l.indexOf('[')+1, l.indexOf(']')).trim());
		l = l.substring(l.indexOf(']')+2);
		row = l.substring(l.indexOf('[')+1, l.indexOf(']')).trim();
		l = l.substring(l.indexOf(']')+2);
		col = l.substring(l.indexOf('[')+1, l.indexOf(']')).trim();
		l = l.substring(l.indexOf(']')+2);
		S = Integer.parseInt(l.substring(l.indexOf('[')+1, l.indexOf(']')).trim());
		l = l.substring(l.indexOf(']')+2);
		Q = Integer.parseInt(l.substring(l.indexOf('[')+1, l.indexOf(']')).trim());
		l = l.substring(l.indexOf(']')+2);
		E = Integer.parseInt(l.substring(l.indexOf('[')+1, l.indexOf(']')).trim());
		l = l.substring(l.indexOf(']')+2);
		T = Integer.parseInt(l.substring(l.indexOf('[')+1, l.indexOf(']')).trim());
		l = l.substring(l.indexOf(']')+2);
		DLY = Integer.parseInt(l.substring(l.indexOf('[')+1, l.indexOf(']')).trim());	
	}
	
	public DramCommand getCAS(){
		DramCommand dc = new DramCommand(this, 0);
		if(this.getType().equalsIgnoreCase("CAS+P"))
			return dc;
		else{//Assumes CASW ends after data restore
			dc.type = "CASW";
			dc.E = this.getS() + DramSimValid.getTimingParameter("tCWD") + 
				DramSimValid.getTimingParameter("tBurst") + DramSimValid.getTimingParameter("tWR");
			dc.T = dc.E - dc.S;
			return dc;
		}
			
	}
	
	public DramCommand getPre(){
		DramCommand dc = new DramCommand(this, 1);
		if(this.getType().equals("CAS+P")){
			return dc;
		}else{//Assumes Pre starts after minimum time to wait and pre ends after array is precharged tRP
			int minimumTimeToWait = DramSimValid.getTimingParameter("tAL") + DramSimValid.getTimingParameter("tBurst") +
			DramSimValid.getTimingParameter("tCWD") + DramSimValid.getTimingParameter("tWR");
			//Assumes Pre starts after the minimum time to wait after the CAS starts
			dc.S = this.getS() + minimumTimeToWait;
			dc.Q = this.getQ();
			//Assumes Pre ends after tRP
			dc.E = dc.S + DramSimValid.getTimingParameter("tRP");
			dc.T = dc.E-dc.S;
			dc.DLY = dc.S-dc.Q;
		}
		return dc;
	}
	
	public String toString(){
		return "F=" + F + " MG="+ MG + " type=" + type + " addr=" + addr + " chan=" + chan +
		" rank=" + rank + " bank=" + bank + " row=" + row + " col=" + col + " S=" + S +
		" Q=" + Q + " E=" + E + " T=" + T + " DLY=" + DLY;
	}
	
	public String getCommand(){
		return command;
	}

	public String getCol() {
		return col;
	}

	public String getType() {
		return type;
	}

	public int getBank() {
		return bank;
	}

	public int getDLY() {
		return DLY;
	}

	public int getE() {
		return E;
	}

	public int getQ() {
		return Q;
	}

	public int getRank() {
		return rank;
	}

	public int getS() {
		return S;
	}

	public int getT() {
		return T;
	}

	public String getAddr() {
		return addr;
	}

	public int getChan() {
		return chan;
	}

	public String getF() {
		return F;
	}

	public int getMG() {
		return MG;
	}

	public String getRow() {
		return row;
	}
}
