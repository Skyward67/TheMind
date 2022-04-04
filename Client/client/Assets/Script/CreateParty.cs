using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;
using UnityEngine.UIElements;

public class CreateParty : MonoBehaviour
{
    public ServerConnection serverConnection;
    public TextField partyName;
    public Dropdown playerNumber;
    
    // Start is called before the first frame update
    void Start()
    {
        //String sent to the server (fields separated with ";");
        string toSend = "CRTE" + ";" + playerNumber.options[playerNumber.value].text + ";" + partyName.text;
        serverConnection.client.send(toSend);
        if (serverConnection.client.getResponse() == "OK")
        {
            SceneManager.LoadScene("Gaming");
        }
    }
}
