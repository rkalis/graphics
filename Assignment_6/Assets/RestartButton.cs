using UnityEngine;
using System.Collections;

public class RestartButton : MonoBehaviour {

	// Use this for initialization
	void Start () {
	
	}

	void restart() {
		Time.timeScale = 1;
		GameOver.outcome = 0;
		Lives.lives = 3;
		Application.LoadLevel (Application.loadedLevel);
	}

	public void onClick() {
		restart ();
	}

	// Update is called once per frame
	void Update () {
	
	}
}
