
public class DramBank
{

  private DramCommand previousCommand;

  public DramBank()
  {

  }

  public String recieveCommand(DramCommand com)
  {
    if (!(previousCommand == null))
      return "";
    else
    {
      previousCommand = com;
      return checkTiming(com);
    }
  }

  public String checkTiming(DramCommand com)
  {
    if (com.getType().equalsIgnoreCase("RAS"))
    { //Prev = A
      if (previousCommand.getType().equals("RAS"))
      { //Next = A
        if (com.getStart() - previousCommand.getEnd() > DramSimValid.getTimingParameter("tRC"))
        {
          return "tRc constraint violated by Command: " + com.toString();
        }
      }
    }
    return "";
  }

}
