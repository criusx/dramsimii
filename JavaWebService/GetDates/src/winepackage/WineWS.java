package winepackage;

import java.rmi.Remote;
import java.rmi.RemoteException;

public interface WineWS
  extends Remote
{
  public String[] getWineAccessesSince(long timestamp)
    throws RemoteException;

  public boolean enterBottleInformation(String[] rfidNums, String type, 
                                        int year, String country, 
                                        String vineyard, String review, byte[] labelImage)
    throws RemoteException;

  public wineBottle retrieveBottleInformation(String rfidNum, 
                                              String phoneID, 
                                              float latitude, 
                                              float longitude)
    throws RemoteException;
}
