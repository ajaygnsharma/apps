/*
	A basic extension of the java.applet.Applet class
 */

import java.awt.*;
import java.applet.*;
import java.net.*;
import java.io.*;


public class iport extends Applet implements Runnable
{
	public void init()
	{
		setLayout(null);
		setBackground(java.awt.Color.lightGray);
		setSize(210,115);
		add(consol);
		consol.setBackground(java.awt.Color.white);
		consol.setBounds(5,60,200,50);
		display.setText("reading");
		display.setAlignment(java.awt.Label.CENTER);
		add(display);
		display.setBackground(java.awt.Color.black);
		display.setForeground(java.awt.Color.green);
		display.setFont(new Font("Dialog", Font.BOLD, 24));
		display.setBounds(5,5,200,50);
		//}}
	}
	
	//{{DECLARE_CONTROLS
	java.awt.TextArea consol = new java.awt.TextArea();
	java.awt.Label display = new java.awt.Label();
	//}}
	
	//Thread repaintThread;
	private Thread	 m_serverThread = null;
	Socket mnSocket=null;	
	PrintWriter out = null;
//	BufferedReader in = null;
    BufferedInputStream in = null;
        
    public void start()
	{
	                  
       /* if (repaintThread == null) {
	    repaintThread = new Thread(this);
	    }
	    repaintThread.start();*/
	    
	    if (m_serverThread == null)
		{
			m_serverThread = new Thread(this);
			m_serverThread.start();
		}		
		if (mnSocket == null)
		{
			try
			{
		//	   mnSocket = new Socket("128.100.101.84", 2000,true);
			mnSocket = new Socket(getCodeBase().getHost(),2000);
			    out = new PrintWriter(mnSocket.getOutputStream());
	         //   in = new BufferedReader(new InputStreamReader(mnSocket.getInputStream()));
	             in = new BufferedInputStream(mnSocket.getInputStream());
	            }catch (UnknownHostException e) {
	                System.err.println("Don't know about host: iServer.");
	           // System.exit(1);
	            }catch (IOException e) {
	                System.err.println("Couldn't get I/O for "
			       + "the connection to:iServer.");
	        //System.exit(1);
			}			
		}
	}
	
		//		The stop() method is called when the page containing the applet is
	// no longer on the screen. The AppletWizard's initial implementation of
	// this method stops execution of the applet's thread.
	//--------------------------------------------------------------------------
	public void stop()
	{
	   // repaintThread.stop();
	   // repaintThread = null;
	    if (m_serverThread != null)
		{
			//m_serverThread.stop();
			m_serverThread = null;
		}
		if (mnSocket != null)
		{
			try
			{
				mnSocket.close();
			}
			catch (Exception e)
			{
				System.out.println("Error closing Socket: "+e);
			}
		}
	}
	
	public void run()
	{ 
	    int size=0;
	    String textdata;
	    textdata=consol.getText();
	    int textsize;
	    textsize=textdata.length();
	    
	    consol.setText("hello world");
	        
	    while (m_serverThread != null)
		{
		    try{
		    m_serverThread.sleep(200);
		    }catch(InterruptedException e){}  
		    
		    if (mnSocket == null)
		    {
			    try
			    {
			 //       mnSocket = new Socket("206.29.25.29", 2003);
				mnSocket = new Socket(getCodeBase().getHost(),2000);
			        out = new PrintWriter(mnSocket.getOutputStream(), true);
	         //   in = new BufferedReader(new InputStreamReader(mnSocket.getInputStream()));
	                in = new BufferedInputStream(mnSocket.getInputStream());
	                }catch (UnknownHostException e) {
	                System.err.println("Don't know about host: iServer.");
	           // System.exit(1);
	                }catch (IOException e) {
	                System.err.println("Couldn't get I/O for "
			       + "the connection to:iServer.");
	        //System.exit(1);
			    }			
		    }
		    if(mnSocket !=null)
		    {
		   try{
		    m_serverThread.sleep(1000);
		    }catch(InterruptedException e){}     
		    
		    String data;            
	        int datasize;
	        data= consol.getText();
	        datasize=data.length();
	        
		//	display.setText(data);
		    if(datasize>textsize)
		    {
		        String outstr;
		        outstr=data.substring(textsize, datasize);
	   //         out.println(outstr);
                if(++size>2000)
                {
                    size=0;
                    consol.setText("");
                }
	            out.print(outstr);
	            out.flush();
	        }		    
		    textsize=datasize;
		      
	        try{
	            byte b[] = new byte[100];
	            int n;
	            n=in.available();
	            if(n>0)
	            {	                           
	                if(in.read(b,0,n)!=0)
	                {
	           // data=in.readLine();
	                    
	                    String indata = new String(b,0,n);
	                    	                
	                    display.setText(indata);
	                    consol.append(indata);
	                    indata=consol.getText();
	                    textsize=indata.length();
	                    if(++size>2000)
                        {
                            size=0;
                           consol.setText("");
                        }
	         //       consol.setText(indata);
	                }	                
	            }
	           
	        }catch (Exception e){
				System.out.println("Error reading from socket: "+e);
			}	
				
           }		   
	//	   Date date= new Date();             
     //      period.setText(date.toString());
	    } 	    
	}    
}
