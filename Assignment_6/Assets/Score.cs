using UnityEngine;
using UnityEngine.UI;
using System.Collections;

public class Score : MonoBehaviour {
	public int score = 0;
	private float scoref = 0.0f;
	private GameObject scoreText;
	// Use this for initialization
	void Start () {
		scoreText = GameObject.Find("/Canvas/Score");
	}

	
	// Update is called once per frame
	void Update () {
		scoref += Time.deltaTime;
		score = (int) Mathf.Floor (scoref);
		scoreText.GetComponent<Text> ().text = "Time: " + score;
	}
}
