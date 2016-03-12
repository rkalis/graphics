using UnityEngine;
using System.Collections;

public class Movement : MonoBehaviour {
	public float speed = 5.0f;
	// Use this for initialization
	void Start () {

	}

	// Update is called once per frame
	void Update () {
		float hor = Input.GetAxisRaw ("Horizontal") * speed * Time.deltaTime;
		Vector3 translation = new Vector3 (hor, 0);
		transform.Translate (translation);
	}
}
