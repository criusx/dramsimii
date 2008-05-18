package petPackage;

import java.rmi.Remote;
import java.rmi.RemoteException;


public interface PetWS
  extends Remote
{


  public boolean changePassword(String RFIDNum, String oldPassword, 
                                String newPassword)
    throws RemoteException;

  public petInfo retrievePetInformation(String rfidNum, String phoneID, 
                                        float latitude, float longitude)
    throws RemoteException;


  public boolean changePasswordTagCode(String tagCode, String oldPassword, 
                                       String newPassword)
    throws RemoteException;


  public petInfo retrievePetInformationTagCode(String tagCode, 
                                               String phoneID, 
                                               float latitude, 
                                               float longitude)
    throws RemoteException;

  public errorMessage updatePetInformationTagCode(petInfo newPet)
    throws RemoteException;

  public errorMessage updatePetInformation(petInfo newPet)
    throws RemoteException;

  public errorMessage authorize(String RFIDNum)
    throws RemoteException;
}
