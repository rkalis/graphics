using UnityEngine;
using UnityEngine.UI;
using System.Collections;

public class GameOver : MonoBehaviour {
	private GameObject gameOverPanel;
	private GameObject gameOverText;
	private GameObject controller;
	private GameObject gameOverButton;
	private GameObject gameOverButtonText;
	public static int outcome = 0;

	// Use this for initialization
	void Start () {
		gameOverPanel      = GameObject.Find ("/Canvas/Game Over Panel");
		gameOverText       = GameObject.Find ("/Canvas/Game Over Panel/Game Over Text");
		gameOverButton     = GameObject.Find ("/Canvas/Game Over Panel/Button");
		gameOverButtonText = GameObject.Find ("/Canvas/Game Over Panel/Button/Text");
		controller         = GameObject.Find ("Controller");
	}
	
	// Update is called once per frame
	void Update () {
		if (outcome != 0) {
			Time.timeScale = 0;
			gameOverText.GetComponent<Text> ().color = new Color(0,0,0,255);
			gameOverPanel.GetComponent<Image> ().color = new Color(255,255,255,255);
			gameOverButton.GetComponent<Image>().color = new Color(180,180,180,255);
			gameOverButtonText.GetComponent<Text> ().color = new Color(0,0,0,255);

			gameOverText.GetComponent<Text>().text = outcome == 1 ? "YOU ROCK" : "YOU LOSE";


		}
	}
}
