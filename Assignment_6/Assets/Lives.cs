using UnityEngine.UI;
using UnityEngine;
using System.Collections;

public class Lives : MonoBehaviour {
	public static int lives = 3;
	private GameObject livesText;
	// Use this for initialization
	void Start () {
		livesText = GameObject.Find("/Canvas/Lives");
	}
	
	// Update is called once per frame
	void Update () {
		livesText.GetComponent<Text> ().text = "Lives: " + lives;
	}
}
