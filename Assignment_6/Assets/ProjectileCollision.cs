using UnityEngine;
using System.Collections;

public class ProjectileCollision : MonoBehaviour {

	// Use this for initialization
	void Start () {
	
	}
	void OnCollisionEnter2D(Collision2D coll) {
		Destroy (gameObject);
	}
	void OnBecameInvisible() {
		Destroy (gameObject);
	}
	
	// Update is called once per frame
	void Update () {
	
	}
}
