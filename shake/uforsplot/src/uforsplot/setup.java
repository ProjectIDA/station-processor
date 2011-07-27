package uforsplot;

import java.awt.BorderLayout;
import java.awt.GridLayout;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;

import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JFormattedTextField;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

public class setup extends JDialog implements ActionListener, FocusListener
{

	/**
   * 1 	Initial distribution
   */
  private static final long   serialVersionUID = 1L;

  private static boolean      bNetChange=false;
  private static boolean      bDisplayChange=false;
  private static boolean      bSmallChange=false;
	private static String       hostname;
	private static String       location;
	private static String       channel1;
  private static String       channel2;
  private static String       channel3;
	private static int          port;
	private static int          secondsDuration;
	private static int          minRange;

	private JButton             okButton;
	private JButton             cancelButton;
	private JFormattedTextField hostField;
	private JFormattedTextField portField;
	private JFormattedTextField channel1Field;
  private JFormattedTextField channel2Field;
  private JFormattedTextField channel3Field;
	private JFormattedTextField locationField;
	private JFormattedTextField secondsField;
  private JFormattedTextField minRangeField;

	private String              saveFocusString;
	private JTextField          statusField;
	public static final int     DEFAULT_WIDTH  = 350;
	public static final int     DEFAULT_HEIGHT = 300;	
	
	public static final double  MAX_SECONDS = 3600*24.0;
	
	public setup(JFrame owner,
			String oldHostname, int oldPort,
	    String oldChannel1, String oldChannel2, String oldChannel3,
	    String oldLocation, int oldSecondsDuration,
	    int oldMinRange)
	{
		super(owner, "Setup Seismic Channel Plot", true);
		
		hostname = oldHostname;
		port = oldPort;
		location = oldLocation;
		channel1 = oldChannel1;
		channel2 = oldChannel2;
		channel3 = oldChannel3;
		secondsDuration = oldSecondsDuration;
		minRange = oldMinRange;
		bNetChange = false;
		bDisplayChange = false;
		bSmallChange = false;
		
		// ======== this ========
		setLayout(new BorderLayout());
		setSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
		
		JPanel panel = new JPanel();
		GridLayout layout = new GridLayout(9,2);
		panel.setLayout(layout);
		panel.setSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);


		hostField = new JFormattedTextField();
		hostField.setValue(oldHostname);
		hostField.setColumns(10);
		hostField.setName("hostField");
		JLabel labelHost = new JLabel("Host:",	JLabel.TRAILING);
		labelHost.setLabelFor(hostField);
		panel.add(labelHost);
		panel.add(hostField);
		hostField.addFocusListener(this);

		portField = new JFormattedTextField();
		portField.setValue(Integer.toString(oldPort, 10));
		portField.setColumns(5);
		portField.setName("portField");
		JLabel labelPort = new JLabel("Port:",	JLabel.TRAILING);
		labelPort.setLabelFor(portField);
		panel.add(labelPort);
		panel.add(portField);
		portField.addFocusListener(this);
		
		channel1Field = new JFormattedTextField();
		channel1Field.setValue(oldChannel1);
		channel1Field.setColumns(10);
		channel1Field.setName("channel1Field");
		JLabel labelChannel1 = new JLabel("Channel 1:",	JLabel.TRAILING);
		labelChannel1.setLabelFor(channel1Field);
		panel.add(labelChannel1);
		panel.add(channel1Field);
		channel1Field.addFocusListener(this);

    channel2Field = new JFormattedTextField();
    channel2Field.setValue(oldChannel2);
    channel2Field.setColumns(10);
    channel2Field.setName("channel2Field");
    JLabel labelChannel2 = new JLabel("Channel 2:",  JLabel.TRAILING);
    labelChannel2.setLabelFor(channel2Field);
    panel.add(labelChannel2);
    panel.add(channel2Field);
    channel2Field.addFocusListener(this);

    channel3Field = new JFormattedTextField();
    channel3Field.setValue(oldChannel3);
    channel3Field.setColumns(10);
    channel3Field.setName("channel3Field");
    JLabel labelChannel3 = new JLabel("Channel 3:",  JLabel.TRAILING);
    labelChannel3.setLabelFor(channel3Field);
    panel.add(labelChannel3);
    panel.add(channel3Field);
    channel3Field.addFocusListener(this);

		locationField = new JFormattedTextField();
		locationField.setValue(oldLocation);
		locationField.setColumns(10);
		locationField.setName("locationField");
		JLabel labelLocation = new JLabel("Location:",	JLabel.TRAILING);
		labelLocation.setLabelFor(locationField);
		panel.add(labelLocation);
		panel.add(locationField);
		locationField.addFocusListener(this);

		secondsField = new JFormattedTextField();
		int iTransition = oldSecondsDuration;
		secondsField.setValue(Double.toString((double)iTransition));
		secondsField.setColumns(6);
		secondsField.setName("secondsField");
		JLabel labelSeconds = new JLabel("Seconds:",	JLabel.TRAILING);
		labelSeconds.setLabelFor(secondsField);
		panel.add(labelSeconds);
		panel.add(secondsField);
		secondsField.addFocusListener(this);
		
    minRangeField = new JFormattedTextField();
    minRangeField.setValue(Integer.toString(oldMinRange));
    minRangeField.setColumns(10);
    minRangeField.setName("minRangeField");
    JLabel labelMinRange = new JLabel("Min Range:",  JLabel.TRAILING);
    labelMinRange.setLabelFor(minRangeField);
    panel.add(labelMinRange);
    panel.add(minRangeField);
    minRangeField.addFocusListener(this);
    
    add(panel, BorderLayout.NORTH);
    
		JPanel midPanel = new JPanel();
		okButton = new JButton("OK");
		okButton.addActionListener(this);
		cancelButton = new JButton("Cancel");
		cancelButton.addActionListener(this);
	
		midPanel.add(okButton);
		midPanel.add(cancelButton);
		add(midPanel, BorderLayout.CENTER);
		
		// Status field
		statusField = new JTextField();
		statusField.setEditable(false);
		statusField.setText("Enter changes");
		statusField.setSize(DEFAULT_WIDTH, 1);
		add(statusField, BorderLayout.SOUTH);
	} // setup() constructor

	public void actionPerformed(ActionEvent e)
	{
		String command = e.getActionCommand();
		Object source = e.getSource();
		if (source == okButton)
		{	
			if (channel1Field.getText().compareToIgnoreCase(channel1) != 0)
			{
				bDisplayChange = true;
				channel1 = channel1Field.getText();
			}
      if (channel2Field.getText().compareToIgnoreCase(channel2) != 0)
      {
        bDisplayChange = true;
        channel2 = channel2Field.getText();
      }
      if (channel3Field.getText().compareToIgnoreCase(channel3) != 0)
      {
        bDisplayChange = true;
        channel3 = channel3Field.getText();
      }
			if (hostField.getText().compareTo(hostname) != 0)
			{
				bNetChange = true;
				hostname = hostField.getText();
			}
			if (port != Integer.parseInt(portField.getText()))
			{
				bNetChange = true;
				port = Integer.parseInt(portField.getText());
			}
			if (location.compareTo(locationField.getText()) != 0)
			{
				bDisplayChange = true;
				location = locationField.getText();
			}
			if (secondsDuration != 
					(int)(Double.parseDouble(secondsField.getText())))
			{
				bSmallChange = true;
				secondsDuration = (int)(Double.parseDouble(secondsField.getText()));
			}
      if (minRange != Integer.parseInt(minRangeField.getText()))
      {
        bSmallChange = true;
        minRange = Integer.parseInt(minRangeField.getText());
      }

			setVisible(false);
		} // okButton was pressed
		else if (source == cancelButton)
		{
			bNetChange = false;
			bDisplayChange = false;
			bSmallChange = false;
  		setVisible(false);
		} else
		{
			System.err.println("Unmanaged setup actionPerformed " + command);
		}
	} // actionPerformed()

	public void focusGained(FocusEvent e)
	{
		JFormattedTextField field = (JFormattedTextField) e.getComponent();

		// Save the current field value in case the user botches up the edit.
		// This allows us to restore the prior value upon field exit
		saveFocusString = field.getText();
	} // focusGained

	public void focusLost(FocusEvent e)
	{
		Object source = e.getSource();
		if (source == portField)
		{
			try
			{
				if (Integer.parseInt(portField.getText()) < 1)
				{
					portField.setText("1");
					statusField.setText("Reset port to minimum value of 1\n");
					Toolkit.getDefaultToolkit().beep();
				}

				if (Integer.parseInt(portField.getText()) > 65535)
				{
					portField.setText(Integer.toString(65535));
					statusField.setText("Reset port to maximum value of 65535\n");
					Toolkit.getDefaultToolkit().beep();
				}
			} catch (NumberFormatException e1)
			{
				statusField.setText("Non integer '" + portField.getText()
				    + "' in port field, restoring former value\n");
				portField.setText(saveFocusString);
				Toolkit.getDefaultToolkit().beep();
			}
		} // if portField
		
		else if (source == secondsField)
		{
			try
			{
				if (Double.parseDouble(secondsField.getText()) < 1.0)
				{
					secondsField.setText("1.0");
					statusField.setText("Reset seconds to minimum value of 1.0\n");
					Toolkit.getDefaultToolkit().beep();
				}

				if (Double.parseDouble(secondsField.getText()) > MAX_SECONDS)
				{
					secondsField.setText(Double.toString(MAX_SECONDS));
					statusField.setText("Reset seconds to maximum value of " 
															+ MAX_SECONDS + "\n");
					Toolkit.getDefaultToolkit().beep();
				}
				
				int wholeSeconds = (int)Double.parseDouble(secondsField.getText());
				secondsField.setText(Double.toString((double)wholeSeconds));
			} catch (NumberFormatException e1)
			{
				statusField.setText("Invalid second '" + secondsField.getText()
				    + "' in seconds field, restoring former value\n");
				secondsField.setText(saveFocusString);
				Toolkit.getDefaultToolkit().beep();
			}
		} // if hoursField
		
    if (source == minRangeField)
    {
      try
      {
        if (Integer.parseInt(minRangeField.getText()) < 1)
        {
          minRangeField.setText("1");
          statusField.setText("Reset min range to minimum value of 1\n");
          Toolkit.getDefaultToolkit().beep();
        }

        if (Integer.parseInt(minRangeField.getText()) > 2000000000)
        {
          minRangeField.setText(Integer.toString(2000000000));
          statusField.setText("Reset min range to maximum value of 2,000,000,000\n");
          Toolkit.getDefaultToolkit().beep();
        }
      } catch (NumberFormatException e1)
      {
        statusField.setText("Non integer '" + minRangeField.getText()
            + "' in min range field, restoring former value\n");
        minRangeField.setText(saveFocusString);
        Toolkit.getDefaultToolkit().beep();
      }
    } // if minRangeField

    else if (source == channel1Field)
    {
      if (channel1Field.getText().length() != 3
          && channel1Field.getText().length() != 0)
      {
        statusField.setText("Channel name '" + channel1Field.getText()
            + "' must be 3 characters or blank, restoring prior name.\n");
        channel1Field.setText(saveFocusString);
        Toolkit.getDefaultToolkit().beep();
      }
      
      if (channel1Field.getText().contains("*") || channel1Field.getText().contains("?"))
      {
        statusField.setText("No channel name wildcards '" + channel1Field.getText()
            + "', restoring prior name.\n");
        channel1Field.setText(saveFocusString);
        Toolkit.getDefaultToolkit().beep();
      }

    } // if channel1Field
        
    else if (source == channel2Field)
    {
      if (channel2Field.getText().length() != 3
          && channel2Field.getText().length() != 0)
      {
        statusField.setText("Channel name '" + channel2Field.getText()
            + "' must be 3 characters or blank, restoring prior name.\n");
        channel2Field.setText(saveFocusString);
        Toolkit.getDefaultToolkit().beep();
      }
      
      if (channel2Field.getText().contains("*") || channel2Field.getText().contains("?"))
      {
        statusField.setText("No channel name wildcards '" + channel2Field.getText()
            + "', restoring prior name.\n");
        channel2Field.setText(saveFocusString);
        Toolkit.getDefaultToolkit().beep();
      }
    } // if channel2Field

    else if (source == channel3Field)
    {
      if (channel3Field.getText().length() != 3
          && channel3Field.getText().length() != 0)
      {
        statusField.setText("Channel name '" + channel3Field.getText()
            + "' must be 3 characters or blank, restoring prior name.\n");
        channel3Field.setText(saveFocusString);
        Toolkit.getDefaultToolkit().beep();
      }
      
      if (channel3Field.getText().contains("*") || channel3Field.getText().contains("?"))
      {
        statusField.setText("No channel name wildcards '" + channel3Field.getText()
            + "', restoring prior name.\n");
        channel3Field.setText(saveFocusString);
        Toolkit.getDefaultToolkit().beep();
      }

    } // if channel3Field

		else if (source == locationField)
		{
			if (locationField.getText().length() != 0
			    && locationField.getText().length() != 2)
			{
				statusField.setText("Location name '" + locationField.getText()
				    + "' must be blank or 2 characters, restoring prior name.\n");
				locationField.setText(saveFocusString);
				Toolkit.getDefaultToolkit().beep();
			}
			if (locationField.getText().length() != 2)
			{
				locationField.setText("  ");
			}
		} // if locationField

	} // focusLost()

	public boolean GetNetChange()
	{
		return bNetChange;
	}
	
	public boolean GetDisplayChange()
	{
		return bDisplayChange;
	}
	
	public boolean GetSmallChange()
	{
		return bSmallChange;
	}
	
	public String GetHostname()
	{
		return hostname;
	}
	
  public String GetChannel1()
  {
    return channel1Field.getText();
  }
  
  public String GetChannel2()
  {
    return channel2Field.getText();
  }
  
  public String GetChannel3()
  {
    return channel3Field.getText();
  }
  
	public String GetLocation()
	{
		return location;
	}
	
	public int GetPort()
	{
		return port;
	}
	
	public int GetSecondsDuration()
	{
		return secondsDuration;
	}
	
  public int GetMinRange()
  {
    return minRange;
  }
  
} // class setup
