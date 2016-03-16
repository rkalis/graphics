using UnityEngine;
using System.Collections;

public class EnemyCollision : MonoBehaviour {

	// Use this for initialization
	void Start () {
	
	}

	void OnCollisionEnter2D(Collision2D coll) {
		if (coll.gameObject.tag == "Player") {
//			Destroy (coll.gameObject);
			Lives.lives--;
			if (Lives.lives <= 0)
				GameOver.outcome = -1;
			Application.LoadLevel(Application.loadedLevel);
		}
	}
	
	// Update is called once per frame
	void Update () {
	}
}
