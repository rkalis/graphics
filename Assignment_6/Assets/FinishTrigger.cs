using UnityEngine;
using System.Collections;

public class FinishTrigger : MonoBehaviour {

	// Use this for initialization
	void Start () {
	
	}

	void OnTriggerEnter2D(Collider2D other) {
		if (other.gameObject.tag == "Player")
			GameOver.outcome = 1;
	}
	
	// Update is called once per frame
	void Update () {
	
	}
}
