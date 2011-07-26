package seed;

/**
 *  A type of exception specific to problems encountered with
 *  Steim compression.
 *
 *  @author Robert Casey
 *  @version 11/20/2002
 */
public class SteimException extends Exception {

	/**
   * 
   */
  private static final long serialVersionUID = 1L;

	public SteimException() {
		super();
	}
	
	public SteimException(String s) {
		super(s);
	}
}

