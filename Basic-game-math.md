|[[Getting Started]] for DX11|[Getting Started](https://github.com/microsoft/DirectXTK12/wiki/Getting-Started) for DX12|
|---|---|

This lesson will cover the basics of 3D transformations for graphics.

# Positioning the camera
We've already used a simple camera setup in the [[3D shapes]] lesson.

In the Game class we added variables for the projection matrix and the view matrix:

```cpp
DirectX::SimpleMath::Matrix m_view;
DirectX::SimpleMath::Matrix m_proj;
```



In the **CreateResources** method, we used the backbuffer size to create a simple perspective view:

```cpp
m_view = Matrix::CreateLookAt(Vector3(2.f, 2.f, 2.f),
    Vector3::Zero, Vector3::UnitY);
m_proj = Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
    float(backBufferWidth) / float(backBufferHeight), 0.1f, 10.f);
```

The ``m_proj`` matrix is created as a perspective camera using an _field-of-view_ of PI/4 radians (which is 45 degrees), the _aspect ratio_ of the backbuffer, the _near-plane_ distance of 0.1 and a _far-plane_ distance of 10.

The ``m_view`` matrix Is created as a view looking from the position (2,2,2) looking at the position (0,0,0) with an _up_ vector of (0,1,0).

These two matrices handle transforming objects positioned in _world_ coordinates and transforming them to _view_ coordinates and then to _screen_ coordinates.

# Positioning an object

The [[3D shapes]] lesson also provides an example of positioning an object.

In the Game class we added a variable for the world matrix for our 3D shape:

```cpp
DirectX::SimpleMath::Matrix m_world;
```

We initially set it to the _identity_ in **CreateDevice**:

```cpp
m_world = Matrix::Identity
```

Then in **Update** we set the matrix to a rotation about the Z-axis based on time:

```cpp
float time = float(timer.GetTotalSeconds());

m_world = Matrix::CreateRotationZ(cosf(time) * 2.f);
```

Then later in the lesson we made it into a rotation about the Y-axes based on time:

```cpp
float time = float(timer.GetTotalSeconds());

m_world = Matrix::CreateRotationY(time);
````

The ``m_world`` matrix transforms the object's _local_ (also known as _model_) coordinates into _world_ coordinates.

# Composing transformations

The power of affine transformation is that you can compose them by multiplying them together. In our [[3D shapes]] lesson, we can modify the **Update**: to get more interesting motion for the planet earth.

For example if you modified **Update** as follows

```cpp
float time = float(timer.GetTotalSeconds());

Vector3 pos = Vector3::Lerp(Vector3::Zero, Vector3::One, cos(time));
m_world = Matrix::CreateRotationY(time) * Matrix::CreateTranslation(pos);
```

Build and run to see the earth spinning and moving away and towards the camera.

If you flipped the order of the matrix multiply, you'll get much different results:

```cpp
float time = float(timer.GetTotalSeconds());

Vector3 pos = Vector3::Lerp(Vector3::Zero, Vector3::One, cos(time));
m_world = Matrix::CreateTranslation( pos ) * Matrix::CreateRotationY( time);
```

Build and run to see the planet earth moving in a more complex pattern.

# Transforming a point

For performance, the majority of the 3D transformations we use in rendering at computed on the GPU using _vertex shaders_ based on the matrices we provide for world, view, and projection. There are times, however, when we want to compute a transformation on the CPU (such as doing collision detection or visibility culling). To transform a point using our variables above we should first concatenate our matrices into a single matrix that combines the transformations.

```cpp
Matrix matrix = m_world * m_view * m_proj;

Vector3 point; // Our input from somewhere

Vector3 newPoint = Vector3::Transform( point, matrix );
```

> Note: In our example we are only transforming a single point which means we are doing 3 matrix multiplies and 1 vector-matrix multiply. For a single point it might have been better to do 3 vector-matrix multiplies, but for most usage cases we are likely to transform more than just a single point. Ideally we'd use the array version of *Transform* to transform many points at once.

# Moving the camera
We can animate the camera's position as well as the object's. In the [[3D shapes]] lesson, we will change **Update** as follows:

```cpp
float time = float(timer.GetTotalSeconds());

m_world = Matrix::CreateRotationY(time);

Vector3 cameraPos = Vector3::Lerp(Vector3(2.f, 2.f, 2.f),
    Vector3(2.f, 0.f, 2.f), cos(time));
m_view = Matrix::CreateLookAt(cameraPos, Vector3::Zero, Vector3::UnitY);
m_effect->SetView(m_view);
```

Build and run to see the camera moving up and down while still looking at the planet earth as the center of focus.

**Next lesson:** [[Collision detection]]

# Further reading

DirectX Tool Kit docs [[SimpleMath]]  
[Radian](http://en.wikipedia.org/wiki/Radian)  
[Graphical projection](http://en.wikipedia.org/wiki/Graphical_projection)  
[Essential Math for Games Programmers (GDC)](http://www.essentialmath.com/)
