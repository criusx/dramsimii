import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;


public class DramChannel
{
  private ArrayList<DramCommand> commands;
  private HashMap<String, Integer> timingParameters;
  private String errors;
  private int lengthOfWindow;

  public DramChannel(HashMap<String, Integer> tPs, int length)
  {
    commands = new ArrayList<DramCommand>();
    commands.ensureCapacity(length);
    lengthOfWindow = length;
    timingParameters = tPs;
  }

  public String checkTiming(DramCommand com)
  {
    commands.add(0, com);
    while (commands.get(commands.size() - 1).getStart() + lengthOfWindow < com.getStart())
    {
      commands.remove(commands.size() - 1);
    }
    errors = "";

    Iterator<DramCommand> itr = commands.iterator();
    DramCommand prev;
    DramCommand now = itr.next();
    //Start of check of all restraints
    while (itr.hasNext())
    {
      prev = itr.next();
      //Start check of all * * a a restraints {R W a a}
      if (prev.getType() == DramCommand.CommandType.READ && now.getType() == DramCommand.CommandType.WRITE)
      {
        if (now.getStart() - prev.getStart() < 
            timingParameters.get("tCAS") + timingParameters.get("tBurst") + timingParameters.get("tRTRS") - 
            timingParameters.get("tCWD"))
          errors += "R W a a restraint violated by \nPrev Command: " + prev + " \nNext Command: " + now + "\n";
      }
      //End check of all * * a a restraints {R W a a}

      //Start check of all * * s * restraints 
      if (prev.getRank() == now.getRank())
      {
        //Start check of all * * s a restraints {R R s a, W R s a, W W s a, F F s a, P F s a}
        //Start check of all * R s a restraints {R R s a, W R s a}
        if (now.getType() == DramCommand.CommandType.READ)
        {
          if (prev.getType() == DramCommand.CommandType.READ)
          { //R R s a
            if ((now.getStart() - prev.getStart()) < 
                Math.max(timingParameters.get("tBurst"), timingParameters.get("tCCD")))
              errors += "R R s a restraint violated by\nPrev Command: " + prev + "\nNext Command: " + now + "\n";
          }
          else if (prev.getType() == DramCommand.CommandType.WRITE)
          { //W R s a 
            if ((now.getStart() - prev.getStart()) < 
                timingParameters.get("tBurst") + timingParameters.get("tCWD") + timingParameters.get("tWTR"))
              errors += "W R s a restraint violated by\nPrev Command: " + prev + "\nNext Command: " + now + "\n";
          }
          //End check of all * R s a restraints {R R s a, W R s a}	
          //Start of check of all * F s a restraints {F F s a, P F s a}
        }
        else if (now.getType() == DramCommand.CommandType.REFRESH)
        {
          if (prev.getType() == DramCommand.CommandType.REFRESH)
          { //F F s a
            if (now.getStart() - prev.getStart() < timingParameters.get("tRFC"))
              errors += "F F s a restraint violated by\nPrev Command: " + prev + "\nNext Command: " + now + "\n";
          }
          else if (prev.getType() == DramCommand.CommandType.PRECHARGE)
          { //P F s a
            if (now.getStart() - prev.getStart() < timingParameters.get("tRP"))
              errors += 
                  "P F s a restraint violated by" + (now.getStart() - prev.getStart() - timingParameters.get("tRP")) + 
                  "\nPrev Command: " + prev + "\nNext Command: " + now + "\n";
          } //End check of all * F s a restraints {F F s a, P F s a}
          //Start check of all * W s a restraints {W W s a}
        }
        else if (now.getType() == DramCommand.CommandType.WRITE)
        {
          if (prev.getType() == DramCommand.CommandType.WRITE) //W W s a
            if (now.getStart() - prev.getStart() < 
                Math.max(timingParameters.get("tBurst"), timingParameters.get("tCCD")))
            {
              errors += "W W s a restraint violated by\nPrev Command: " + prev + "\nNext Command: " + now + "\n";
            }
        } //End of check of all * * s a restraints {R R s a, W R s a, W W s a, F F s a, P F s a}
        //Start of check of all * * s s restraints {A A s s, F A s s, A R s s, A W s s, A P s s, R P s s, W P s s}
        if (prev.getBank() == now.getBank())
        {
          //Start of check of all * A s s restraints {A A s s, F A s s}
          if (now.getType() == DramCommand.CommandType.ACTIVATE)
          {
            if (prev.getType() == DramCommand.CommandType.ACTIVATE)
            { //A A s s
              if (now.getStart() - prev.getStart() < timingParameters.get("tRC"))
                errors += "A A s s (tRC = " + timingParameters.get("tRC") + ") restraint violated by\nPrev Command: " + prev + "\nNext Command: " + now + "\n";

            }
            else if (prev.getType() == (DramCommand.CommandType.PRECHARGE))
            { //P A s s
              if (now.getStart() - prev.getStart() < timingParameters.get("tRP"))
                errors += 
                    "P A s s restraint violated by (" + (now.getStart() - prev.getStart() - timingParameters.get("tRP")) + 
                    ")\nPrev Command: " + prev + "\nNext Command: " + now + "\n";
            }
            else if (prev.getType() == (DramCommand.CommandType.REFRESH))
            { //F A s s
              if (now.getStart() - prev.getStart() < timingParameters.get("tRFC"))
                errors += "F A s s restraint violated by\nPrev Command: " + prev + "\nNext Command: " + now + "\n";
            }
            //End of check of all * A s s restraints {A A s s, F A s s}
            //Start of check of all * R s s restraints {A R s s}
          }
          else if (now.getType() == (DramCommand.CommandType.READ))
          {
            if (prev.getType() == DramCommand.CommandType.ACTIVATE)
            { //A R s s
              if (now.getStart() - prev.getStart() < timingParameters.get("tRCD") - timingParameters.get("tAL"))
                errors += "A R s s restraint violated by\nPrev Command: " + prev + "\nNext Command: " + now + "\n";
            }
            //End of check of all * R s s restraints {A R s s}
            //Start of check of all * W s s restraints {A W s s}
          }
          else if (now.getType() == (DramCommand.CommandType.WRITE))
          {
            if (prev.getType() == DramCommand.CommandType.ACTIVATE)
            { //A W s s
              if (now.getStart() - prev.getStart() < timingParameters.get("tRCD") - timingParameters.get("tAL"))
                errors += "A W s s restraint violated by\nPrev Command: " + prev + "\nNext Command: " + now + "\n";
            }
            //End of check of all * W s s restraints {A W s s}
            //Start of check of all * P s s restraints {A P s s, R P s s, W P s s}
          }
          else if (now.getType() == (DramCommand.CommandType.PRECHARGE))
          {
            if (prev.getType() == DramCommand.CommandType.ACTIVATE)
            { //A P s s
              if (now.getStart() - prev.getStart() < timingParameters.get("tRAS"))
                errors += 
                    "A P s s (tRAS=\" + timingParameters.get(\"tRAS\") + \") restraint violated by\nPrev Command: " + 
                    prev + "\nNext Command: " + now + "\n";
            }
            else if (prev.getType() == (DramCommand.CommandType.READ))
            { //R P s s
              if (now.getStart() - prev.getStart() < 
                  timingParameters.get("tBurst") + timingParameters.get("tRTP") - timingParameters.get("tCCD") + 
                  timingParameters.get("tAL"))
                errors += "R P s s restraint violated by\nPrev Command: " + prev + "\nNext Command: " + now + "\n";
            }
            else if (prev.getType() == (DramCommand.CommandType.WRITE))
            { //W P s s
              if (now.getStart() - prev.getStart() < 
                  timingParameters.get("tCWD") + timingParameters.get("tBurst") + timingParameters.get("tWR") + 
                  timingParameters.get("tAL"))
                errors += 
                    "W P s s restraint violated by " + (now.getStart() - prev.getStart() - (timingParameters.get("tCWD") + 
                                                                                            timingParameters.get("tBurst") + 
                                                                                            timingParameters.get("tWR")) + 
                                                        "\nPrev Command: " + prev + "\nNext Command: " + now + "\n");
            }
            //End of check of all * P s s restriants {A P s s, R P s s, W P s s}
          }
          //End of check of all * * s s restraints {A A s s, F A s s, A R s s, A W s s, A P s s, R P s s, W P s s}
        }
        else
        { //Start of check of all * * s d restraints {A A s d, P A s d}
          //Start of check of all * A s d restraitns {A A s d, P A s d}
          if (now.getType() == DramCommand.CommandType.ACTIVATE)
          {
            if (prev.getType() == DramCommand.CommandType.ACTIVATE)
            { //A A s d
              if (now.getStart() - prev.getStart() < timingParameters.get("tRRD"))
                errors += 
                    "A A s d (tRRD=" + timingParameters.get("tRRD") + ") restraint violated by\nPrev Command: " + prev + 
                    "\nNext Command: " + now + "\n";
            }

            //End of check of all * A s d restraints {A A s d, P A s d}
          }
          //End of check of all * * s d restraints {A A s d, P A s d}
        }
        //End of check of all * * s * restraints
      }
      else
      { //Start of check of all * * d * restraints {R R d a, W R d a, W W d a}
        //Start of check of all * * d a restraints {R R d a, W R d a, W W d a}
        //Start of check of all * R d a restraints {R R d a, W R d a}
        if (now.getType() == (DramCommand.CommandType.READ))
        {
          if (prev.getType() == (DramCommand.CommandType.READ))
          { //R R d a
            if (now.getStart() - prev.getStart() < timingParameters.get("tBurst") + timingParameters.get("tRTRS"))
              errors += "R R d a restraint violated by\nPrev Command: " + prev + "\nNext Command: " + now + "\n";
          }
          else if (prev.getType() == (DramCommand.CommandType.WRITE))
          { //W R d a
            if (now.getStart() - prev.getStart() < 
                timingParameters.get("tCWD") + timingParameters.get("tBurst") + timingParameters.get("tRTRS") - 
                timingParameters.get("tCAS"))
              errors += "W R d a restraint violated by\nPrev Command: " + prev + "\nNext Command: " + now + "\n";
          }
          //End of check of all * R d a restraints {R R d a, W R d a}
          //Start of check of all * W d a restraints {W W d a}
        }
        else if (now.getType() == (DramCommand.CommandType.WRITE))
        {
          if (prev.getType() == (DramCommand.CommandType.WRITE))
          { //W W d a
            if (now.getStart() - prev.getStart() < timingParameters.get("tBurst") + timingParameters.get("tOST"))
              errors += "W W d a restraint violated by\nPrev Command: " + prev + "\nNext Command: " + now + "\n";
          }
          //End of check of all * W d a restraints {W W d a}
        }
        //End of check of all * * d a restraints {R R d a, W R d a, W W d a}
        //End of check of all * * d * restraints {R R d a, W R d a, W W d a}
      }
    } //End of check of all restraints	

    return errors;
  }
}
