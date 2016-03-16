using UnityEngine;
using System.Collections;

public class ProjectileCollision : MonoBehaviour {

	// Use this for initialization
	void Start () {
	
	}
	void OnCollisionEnter2D(Collision2D coll) {
		if (coll.gameObject.tag == "Enemy") {
			coll.gameObject.GetComponent<AudioSource> ().Play ();
			coll.gameObject.GetComponent<Renderer> ().enabled = false;
			Destroy (coll.gameObject, coll.gameObject.GetComponent<AudioSource> ().clip.length);
		}
		Destroy (gameObject);
	}
	void OnBecameInvisible() {
		Destroy (gameObject, 1.0f);
	}
	
	// Update is called once per frame
	void Update () {
	
	}
}
