import java.io.IOException;


public class DramSimTest {

	/**
	 * @param args[0] = Command inputs
	 * @param args[1] = timing parameters input
	 * @param args[2] = outfile name
	 */
	
	public static void main(String[] args) {
		try {
			DramSimValid dsv = new DramSimValid(args[0],
					args[1],
					args[2]);
					
		} catch (IOException e) {
			System.out.println("IOEXCEPTION"+ e.getLocalizedMessage() +e.getMessage()+e.getCause());
		}

	}

}
