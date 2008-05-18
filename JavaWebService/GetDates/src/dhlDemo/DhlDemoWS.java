package dhlDemo;

import java.rmi.Remote;
import java.rmi.RemoteException;


public interface DhlDemoWS
  extends Remote
{
  public packageInfo scanPackage(packageScan newScan)
    throws RemoteException;

  public boolean setPackageInfo(packageInfo newPackage)
    throws RemoteException;

  public packageInfo getPackageInfo(String RFIDNum)
    throws RemoteException;
}
