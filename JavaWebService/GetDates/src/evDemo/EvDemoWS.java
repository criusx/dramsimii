package evDemo;

import java.rmi.Remote;
import java.rmi.RemoteException;

public interface EvDemoWS
  extends Remote
{
  public boolean addSpectrum(spectrumInfo newSample)
    throws RemoteException;

  public spectrumInfo getSpectrum(String RFIDNum)
    throws RemoteException;
}
