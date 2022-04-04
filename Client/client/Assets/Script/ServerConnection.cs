/**
link to Microsoft documentation : https://docs.microsoft.com/en-us/dotnet/framework/network-programming/asynchronous-client-socket-example
 */

using System.Collections;
using System.Collections.Generic;
using UnityEngine;

using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Text;
using UnityEngine.SceneManagement;
using UnityEngine.UI;
using UnityEngine.UIElements;

// State object for receiving data from remote device.  
public class StateObject
{
    // Client socket.  
    public Socket workSocket = null;
    // Size of receive buffer.  
    public const int BufferSize = 5;
    // Receive buffer.  
    public byte[] buffer = new byte[BufferSize];
    // Received data string.  
    public StringBuilder sb = new StringBuilder();
}

public class AsynchronousClient
{
    // The port number for the remote device.  
    private static int port;

    // ManualResetEvent instances signal completion.  
    private static ManualResetEvent connectDone =
        new ManualResetEvent(false);
    private static ManualResetEvent sendDone =
        new ManualResetEvent(false);
    private static ManualResetEvent receiveDone =
        new ManualResetEvent(false);

    // The response from the remote device.  
    private String response = String.Empty;

    private Socket client;

    public AsynchronousClient(string _ip, int _port)
    {
        startClient(_ip, _port);
    }

    public void startClient(string _ip, int _port)
    {
        port = _port;
        try
        {
            IPAddress ipAddress = IPAddress.Parse(_ip);
            IPEndPoint remoteEP = new IPEndPoint(ipAddress, port);

            this.client = new Socket(ipAddress.AddressFamily,
                SocketType.Stream, ProtocolType.Tcp);

            client.BeginConnect(remoteEP,
                new AsyncCallback(ConnectCallback), client);
        }
        catch (Exception e)
        {
            Console.WriteLine(e.ToString());
        }
    }

    /*
    public static void StartClient(string _ip, int _port)
    {
        port = _port;
        // Connect to a remote device.  
        try
        {
            // Establish the remote endpoint for the socket.  
            // The name of the
            // remote device is "host.contoso.com".  

            //IPHostEntry ipHostInfo = Dns.GetHostEntry("host.contoso.com");
            //IPAddress ipAddress = ipHostInfo.AddressList[0];
            IPAddress ipAddress = IPAddress.Parse(_ip);
            IPEndPoint remoteEP = new IPEndPoint(ipAddress, port);

            // Create a TCP/IP socket.  
            Socket client = new Socket(ipAddress.AddressFamily,
                SocketType.Stream, ProtocolType.Tcp);

            // Connect to the remote endpoint.  
            client.BeginConnect(remoteEP,
                new AsyncCallback(ConnectCallback), client);
            
            //connectDone.WaitOne();

            // Send test data to the remote device.  
            Send(client, "This is a test<EOF>");
            //sendDone.WaitOne();

            // Receive the response from the remote device.  
            Receive(client);
            //receiveDone.WaitOne();

            // Write the response to the console.  
            Console.WriteLine("Response received : {0}", response);
            
            // Release the socket.  
            client.Shutdown(SocketShutdown.Both);
            client.Close();

        }
        catch (Exception e)
        {
            Console.WriteLine(e.ToString());
        }
    }
    */
    

    public void deleteClient()
    {
        try
        {
            client.Shutdown(SocketShutdown.Both);
            client.Close();
        }
        catch (Exception e)
        {
            Console.WriteLine(e.ToString());
        }
    }

    private static void ConnectCallback(IAsyncResult ar)
    {
        try
        {
            // Retrieve the socket from the state object.  
            Socket client = (Socket)ar.AsyncState;

            // Complete the connection.  
            client.EndConnect(ar);

            Console.WriteLine("Socket connected to {0}",
                client.RemoteEndPoint.ToString());

            // Signal that the connection has been made.  
            connectDone.Set();
        }
        catch (Exception e)
        {
            Console.WriteLine(e.ToString());
        }
    }

    public void receive()
    {
        try
        {
            // Create the state object.  
            StateObject state = new StateObject();
            state.workSocket = client;

            // Begin receiving the data from the remote device.  
            client.BeginReceive(state.buffer, 0, StateObject.BufferSize, 0,
                new AsyncCallback(ReceiveCallback), state);
        }
        catch (Exception e)
        {
            Console.WriteLine(e.ToString());
        }
    }

    private void ReceiveCallback(IAsyncResult ar)
    {
        try
        {
            
            // Retrieve the state object and the client socket
            // from the asynchronous state object.  
            StateObject state = (StateObject)ar.AsyncState;
            //Socket client = state.workSocket;

            // Read data from the remote device.  
            int bytesRead = client.EndReceive(ar);

            if (bytesRead > 0)
            {
                // There might be more data, so store the data received so far.  
                state.sb.Append(Encoding.ASCII.GetString(state.buffer, 0, bytesRead));
                
                // Get the rest of the data.  
                if (bytesRead < 5)
                {
                    // All the data has arrived; put it in response.  
                    if (state.sb.Length > 1)
                    {
                        response = state.sb.ToString();
                    }
                    // Signal that all bytes have been received.  
                    receiveDone.Set();
                }
                else
                {
                    client.BeginReceive(state.buffer, 0, StateObject.BufferSize, 0,
                        new AsyncCallback(ReceiveCallback), state);
                    
                }
            }
            else
            {
                // All the data has arrived; put it in response.  
                if (state.sb.Length > 1)
                {
                    response = state.sb.ToString();
                }
                // Signal that all bytes have been received.  
                receiveDone.Set();
            }
        }
        catch (Exception e)
        {
            Console.WriteLine(e.ToString());
        }
        
        
    }

    public void send(String data)
    {
        // Convert the string data to byte data using ASCII encoding.  
        byte[] byteData = Encoding.ASCII.GetBytes(data);

        // Begin sending the data to the remote device.  
        client.BeginSend(byteData, 0, byteData.Length, 0,
            new AsyncCallback(SendCallback), client);
    }

    private static void SendCallback(IAsyncResult ar)
    {
        try
        {
            // Retrieve the socket from the state object.  
            Socket client = (Socket)ar.AsyncState;

            // Complete sending the data to the remote device.  
            int bytesSent = client.EndSend(ar);
            Console.WriteLine("Sent {0} bytes to server.", bytesSent);

            // Signal that all bytes have been sent.  
            sendDone.Set();
        }
        catch (Exception e)
        {
            Console.WriteLine(e.ToString());
        }
    }

    public String getResponse()
    {
        return response;
    }

    public void setResponse(String _response)
    {
        response = _response;
    }
}

public class ServerConnection : MonoBehaviour
{

    public String ipAddress = "0.0.0.0";
    public int port = 3490;
    public static AsynchronousClient client;
    
    public InputField partyName;
    public Dropdown playerNumber;

    public Dropdown partyList;

    private void Awake()
    {
        DontDestroyOnLoad(this);
    }

    // Start is called before the first frame update
    void Start()
    {
        client = new AsynchronousClient(ipAddress, port);
        //client.send("aojfnsfi");
    }

    private void Update()
    {
        client.receive();
        if (client.getResponse() != "")
        {
            Debug.Log(client.getResponse());
        }
        client.setResponse("");
    }

    private void OnApplicationQuit()
    {
        client.deleteClient();   
    }

    public void joinChannel()
    {
        string toSend = "JOIN;\n";
        client.send(toSend);
        bool isResponse = false;
        while (!isResponse)
        {
            if (client.getResponse() == "OK")
            {
                Debug.Log(client.getResponse());
                isResponse = true;
                client.setResponse("");
                
            } 
        }
    }

    public void createChannel()
    {
        //String sent to the server (fields separated with ";");
        string toSend = "CRTE" + ";" + playerNumber.options[playerNumber.value].text + ";" + partyName.text;
        Debug.Log(toSend);
        client.send(toSend);
        bool isResponse = false;
        while (!isResponse)
        {
            if (client.getResponse().Split(';')[0] == "LIST")
            {
                isResponse = true;
                for (int i = 1; i < client.getResponse().Split(';').Length; i++)
                {
                    partyList.options.Add(new Dropdown.OptionData(client.getResponse().Split(';')[i]));
                }
                client.setResponse("");
                //TODO start waiting room
            } 
        }
        
    }

    public void justToKnow()
    {
        client.send("test");
    }

}
