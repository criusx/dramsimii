package assayPackage;

import java.rmi.Remote;
import java.rmi.RemoteException;


public interface AssayWS
  extends Remote
{
  public assayInfo retrieveAssayInformation(String rfidNum, String phoneID)
    throws RemoteException;

  public assayResultResponse submitAssayResult(String sessionID, 
                                               String phoneID, 
                                               int imageChosen)
    throws RemoteException;
}
