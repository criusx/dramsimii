import java.util.HashMap;

public class DramCommand
{

  public enum CommandType
  {
    ACTIVATE,
    PRECHARGE,
    READ,
    READ_AND_PRECHARGE,
    WRITE,
    WRITE_AND_PRECHARGE,
    REFRESH,
    ;
  }

  private String command;
  private static HashMap<Integer, Long> lastActivateTime = new HashMap<Integer, Long>();
  private String addr;
  private CommandType type;
  private int F, col, row;
  private int MG, rank, bank, chan;
  private long originalStart, start, enqueue, end, T, DLY;

  //This constructor takes a CAS+P command and returns only the CAS part or the Pre part depending on type
  //if type = 0 return CAS else return Pre

  private DramCommand(DramCommand casp, int type)
  {
    originalStart = casp.getStart();

    switch (type)
    {
      case 0: //Return CAS Assumes end time is end of data burst
        F = casp.getF();
        MG = casp.getMG();
        this.type = CommandType.READ;
        addr = casp.getAddr();
        chan = casp.getChan();
        rank = casp.getRank();
        bank = casp.getBank();
        row = casp.getRow();
        col = casp.getCol();
        //start = casp.getStart() + DramSimValid.getTimingParameter("tAL");
        start = casp.getStart();
        enqueue = casp.getEnqueue();
        end = DramSimValid.getTimingParameter("tCAS") + DramSimValid.getTimingParameter("tBurst") + start;
        T = end - start;
        DLY = start - enqueue;
        break;
      default: //return Pre Assume end of Pre is at end of array precharge tRP
        F = casp.getF();
        MG = casp.getMG();
        this.type = CommandType.PRECHARGE;
        addr = casp.getAddr();
        chan = casp.getChan();
        rank = casp.getRank();
        bank = casp.getBank();
        row = casp.getRow();
        col = casp.getCol();
        /*System.out.println(DramSimValid.getTimingParameter("tAL") + "\t" +
                           DramSimValid.getTimingParameter("tBurst") + "\t" +
                           DramSimValid.getTimingParameter("tRTP") + "\t" +
                           DramSimValid.getTimingParameter("tCCD"));*/
        // DRAM will internally delay until tRAS is met
        int codedAddress = (((10 * chan) + rank) * 100) + bank;
        long RASlockoutTime = lastActivateTime.get(codedAddress) + DramSimValid.getTimingParameter("tRAS");
        long minimumTimeToWait = 
          DramSimValid.getTimingParameter("tAL") + DramSimValid.getTimingParameter("tBurst") + DramSimValid.getTimingParameter("tRTP") - 
          DramSimValid.getTimingParameter("tCCD");
        //Assumes Pre starts after the minimum time to wait after the CAS starts
        start = Math.max(casp.getStart() + minimumTimeToWait, RASlockoutTime);
        enqueue = casp.getEnqueue();
        //Assumes Pre ends after tRP
        end = start + DramSimValid.getTimingParameter("tRP");
        T = end - start;
        DLY = start - enqueue;
    }

  }

  public DramCommand(String l)
  {
    try
    {
      l = l.substring(2);
      command = l.substring(0, l.indexOf(' '));
      //F = Integer.parseInt(l.substring(l.indexOf('[') + 1, l.indexOf(']')).trim(), 16);
      //l = l.substring(l.indexOf(']') + 2);
      //MG = Integer.parseInt(l.substring(l.indexOf('[') + 1, l.indexOf(']')).trim(), 16);
      //l = l.substring(l.indexOf(']') + 2);
      String type = l.substring(0, l.indexOf("addr")).trim();
      setCommandType(type);
      l = l.substring(type.length());
      addr = l.substring(l.indexOf('[') + 1, l.indexOf(']')).trim();
      l = l.substring(l.indexOf(']') + 2);
      chan = Integer.parseInt(l.substring(l.indexOf('[') + 1, l.indexOf(']')).trim(), 16);
      l = l.substring(l.indexOf(']') + 2);
      rank = Integer.parseInt(l.substring(l.indexOf('[') + 1, l.indexOf(']')).trim(), 16);
      l = l.substring(l.indexOf(']') + 2);
      bank = Integer.parseInt(l.substring(l.indexOf('[') + 1, l.indexOf(']')).trim(), 16);
      l = l.substring(l.indexOf(']') + 2);
      row = Integer.parseInt(l.substring(l.indexOf('[') + 1, l.indexOf(']')).trim(), 16);
      l = l.substring(l.indexOf(']') + 2);
      col = Integer.parseInt(l.substring(l.indexOf('[') + 1, l.indexOf(']')).trim(), 16);
      l = l.substring(l.indexOf(']') + 2);
      enqueue = Long.parseLong(l.substring(l.indexOf('[') + 1, l.indexOf(']')).trim(), 10);
      l = l.substring(l.indexOf(']') + 2);
      originalStart = start = Long.parseLong(l.substring(l.indexOf('[') + 1, l.indexOf(']')).trim(), 10);
      l = l.substring(l.indexOf(']') + 2);
      end = Long.parseLong(l.substring(l.indexOf('[') + 1, l.indexOf(']')).trim(), 10);
      l = l.substring(l.indexOf(']') + 2);
      T = Integer.parseInt(l.substring(l.indexOf('[') + 1, l.indexOf(']')).trim(), 10);
      l = l.substring(l.indexOf(']') + 2);
      DLY = Integer.parseInt(l.substring(l.indexOf('[') + 1, l.indexOf(']')).trim(), 10);

      if (type.compareToIgnoreCase("RAS") == 0)
      {
        int codedAddress = (((10 * chan) + rank) * 100) + bank;
        lastActivateTime.put(codedAddress, start);
      }
    }
    catch (NumberFormatException e)
    {
      e.printStackTrace();
    }
  }

  private void setCommandType(String commandString)
  {
    if (commandString.compareToIgnoreCase("RAS") == 0)
      type = CommandType.ACTIVATE;
    else if (commandString.compareToIgnoreCase("PREC") == 0)
      type = CommandType.PRECHARGE;
    else if (commandString.compareToIgnoreCase("REF") == 0)
      type = CommandType.REFRESH;
    else if (commandString.compareToIgnoreCase("CAS") == 0)
      type = CommandType.READ;
    else if (commandString.compareToIgnoreCase("CASW") == 0)
      type = CommandType.WRITE;
    else if (commandString.compareToIgnoreCase("CAS+P") == 0)
      type = CommandType.READ_AND_PRECHARGE;
    else if (commandString.compareToIgnoreCase("CASW+P") == 0)
      type = CommandType.WRITE_AND_PRECHARGE;
    else
      throw new IllegalArgumentException();

  }

  public DramCommand getCAS()
  {
    DramCommand dc = new DramCommand(this, 0);

    if (this.getType() == CommandType.READ_AND_PRECHARGE)
      return dc;
    else
    { //Assumes CASW ends after data restore
      dc.type = CommandType.WRITE;
      dc.end = 
          this.getStart() + DramSimValid.getTimingParameter("tCWD") + DramSimValid.getTimingParameter("tBurst") + DramSimValid.getTimingParameter("tWR");
      dc.T = dc.end - dc.start;
      return dc;
    }

  }

  public DramCommand getPre()
  {
    DramCommand dc = new DramCommand(this, 1);
    if (this.getType().equals("CAS+P"))
    {
      return dc;
    }
    else
    { //Assumes Pre starts after minimum time to wait and pre ends after array is precharged tRP
      int minimumTimeToWait = 
        DramSimValid.getTimingParameter("tAL") + DramSimValid.getTimingParameter("tBurst") + DramSimValid.getTimingParameter("tCWD") + 
        DramSimValid.getTimingParameter("tWR");
      //Assumes Pre starts after the minimum time to wait after the CAS starts
      dc.start = this.getStart() + minimumTimeToWait;
      dc.enqueue = this.getEnqueue();
      //Assumes Pre ends after tRP
      dc.end = dc.start + DramSimValid.getTimingParameter("tRP");
      dc.T = dc.end - dc.start;
      dc.DLY = dc.start - dc.enqueue;
    }
    return dc;
  }

  public String toString()
  {
    return "F=" + F + " MG=" + MG + " type=" + type + " addr=" + addr + " chan=" + chan + " rank=" + rank + " bank=" + 
      bank + " row=" + row + " col=" + col + " OS=" + originalStart + " S=" + start + " Q=" + enqueue + " E=" + end + 
      " T=" + T + " DLY=" + DLY;
  }

  public String getCommand()
  {
    return command;
  }

  public int getCol()
  {
    return col;
  }

  public CommandType getType()
  {
    return type;
  }

  public int getBank()
  {
    return bank;
  }

  public long getDLY()
  {
    return DLY;
  }

  public long getEnd()
  {
    return end;
  }

  public long getEnqueue()
  {
    return enqueue;
  }

  public int getRank()
  {
    return rank;
  }

  public long getStart()
  {
    return start;
  }

  public long getT()
  {
    return T;
  }

  public String getAddr()
  {
    return addr;
  }

  public int getChan()
  {
    return chan;
  }

  public int getF()
  {
    return F;
  }

  public int getMG()
  {
    return MG;
  }

  public int getRow()
  {
    return row;
  }
}
