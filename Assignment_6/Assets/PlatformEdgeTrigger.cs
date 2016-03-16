using UnityEngine;
using System.Collections;

public class PlatformEdgeTrigger : MonoBehaviour {

	// Use this for initialization
	void Start () {
	
	}

	void OnTriggerEnter2D(Collider2D other) {
		if (other.gameObject.tag == "Enemy") {
			other.gameObject.GetComponent<EnemyMovement> ().direction *= -1;
		}
	}
	
	// Update is called once per frame
	void Update () {
	
	}
}
