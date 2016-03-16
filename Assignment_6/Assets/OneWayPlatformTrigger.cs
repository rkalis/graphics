using UnityEngine;
using System.Collections;

public class OneWayPlatformTrigger : MonoBehaviour {
	public GameObject platform;

	// Use this for initialization
	void Start () {
	
	}

	void OnTriggerEnter2D(Collider2D other) {
		if (other.gameObject.tag == "Player") {
			other.gameObject.GetComponent<BoxCollider2D>().isTrigger = true;
		}
	}

	void OnTriggerExit2D(Collider2D other) {
		if (other.gameObject.tag == "Player") {
			other.gameObject.GetComponent<BoxCollider2D>().isTrigger = false;
		}
	}
	
	// Update is called once per frame
	void Update () {
	
	}
}
