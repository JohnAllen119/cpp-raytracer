## 1. Project Introduction

### Q1: 请用一句话介绍这个项目

这是一个使用 C++ 从零实现的学习型 Ray Tracer / Path Tracer，支持基础几何求交、递归材质散射、场景级 BVH、OBJ 模型导入、UV 纹理和顶点法线平滑着色。

---

### Q2: 请详细介绍这个项目

这是一个使用 C++ 从零实现的学习型 Ray Tracer / Path Tracer。

项目支持 Sphere、Plane、Triangle 和 OBJ Mesh 等几何对象，并通过统一的 Object 求交接口参与场景渲染。

材质部分使用 Material 基类和 scatter() 虚函数进行抽象，目前实现了 Lambertian、Metal 和 Dielectric 等材质。compute_color() 负责递归追踪光线，并根据材质返回的 scattered ray 和 attenuation 计算最终颜色。

项目实现了场景级 BVH，用于减少光线与场景物体之间不必要的求交测试。OBJ 导入管线支持 v、vt 和 vn，Mesh 会把顶点位置、纹理坐标和顶点法线传递给 Triangle。光线命中 Triangle 后，通过重心坐标插值得到 UV 和平滑法线，再将这些数据传给材质和纹理采样流程。

该项目的主要目的不是实现工业级渲染器，而是训练 C++ 面向对象设计、几何求交、递归算法、加速结构、OBJ 资产导入和渲染数据流分析能力。

---

### Q3: 为什么选择做 Ray Tracer

我选择 Ray Tracer，是因为它能够把 C++、数学、数据结构和软件设计结合在一个完整项目中。

这个项目不仅需要实现向量运算和光线与几何体求交，还需要处理继承、多态、内存管理、递归、文件解析、BVH 和模块之间的数据传递。相比只完成相互独立的算法题，它更适合训练我理解一个完整程序怎样从输入数据运行到最终输出。

我并不打算把它包装成工业级渲染器，也不以图形学岗位作为唯一方向。这个项目对我的价值主要在于建立较完整的 C++ 工程经验，并训练定位问题、扩展功能和解释系统数据流的能力。

## 2. Overall Architecture


### Q1: 项目的整体数据流是什么

项目可以分为资产加载、场景构建、光线生成、求交、材质散射和图像输出几个阶段。

整体数据流如下：

1. OBJLoader 读取 OBJ 文件中的 v、vt、vn 和 face 数据。
2. OBJData 保存解析得到的顶点、纹理坐标、顶点法线和面索引。
3. Mesh 根据 OBJData 创建 Triangle，并把对应的 UV 和顶点法线传给 Triangle。
4. main.cpp 创建相机、材质和场景物体，并将物体加入场景。
5. 场景中的 Object 被用于构建场景级 BVH。
6. 相机为每个像素生成 primary ray。
7. 光线通过 BVH 查找最近命中的 Object。
8. 如果命中 Mesh，Mesh 会继续测试内部 Triangle。
9. 命中信息被保存到 hit_record，包括 t、命中点、法线、材质以及 UV。
10. compute_color() 调用命中材质的 scatter()，得到 scattered ray 和 attenuation。
11. compute_color() 递归追踪 scattered ray，并计算当前光线的返回颜色。
12. 对每个像素的多次采样结果取平均，再进行 Gamma 校正和颜色范围限制。
13. 最终将像素颜色写入 PPM 图像文件。

简化后的渲染路径是：

Camera → Ray → Scene BVH → Object / Mesh → Triangle → hit_record → Material::scatter() → compute_color() → Image Output

---

### Q2: 为什么需要 hit_record

hit_record 用于统一保存一次光线命中的结果。

光线与不同物体求交时，Sphere、Plane、Triangle 和 Mesh 的求交算法不同，但后续的材质散射和颜色计算需要使用相同类型的数据。因此，求交函数不能只返回 true 或 false，还需要把命中位置和表面属性传递给后续模块。

hit_record 通常保存：

1. 参数 t，用于比较多个命中点并选择距离光线起点最近的命中。
2. 命中点 p。
3. 命中点的表面法线 normal。
4. 命中物体的 material。
5. 纹理坐标 u 和 v。

对于 Triangle，UV 和平滑法线还需要根据命中点的重心坐标进行插值，然后写入 hit_record。

hit_record 的作用是把几何求交和材质计算连接起来。求交模块负责填写命中信息，compute_color() 和 Material::scatter() 负责使用这些信息，不需要了解具体命中了哪一种几何体。

---

### Q3: compute_color 的作用是什么

compute_color() 负责计算一条 ray 最终返回的颜色，是递归路径追踪的核心函数。

它的主要流程是：

1. 判断递归深度是否已经达到上限。
2. 使用 ray 与场景求交。
3. 如果没有命中物体，返回背景颜色。
4. 如果命中物体，从 hit_record 中取得命中点、法线、材质和 UV。
5. 调用 material->scatter()。
6. scatter() 返回是否继续散射，并设置 scattered ray 和 attenuation。
7. 如果散射成功，递归调用 compute_color() 计算 scattered ray 的颜色。
8. 将递归返回的颜色与 attenuation 相乘，得到当前 ray 的颜色。
9. 如果材质不再散射，则返回黑色或材质对应的终止结果。

简化表达为：

current color = attenuation × compute_color(scattered ray)

compute_color() 负责控制递归和组合颜色，但不负责决定具体材质怎样散射。

---

### Q4: 为什么使用 Material::scatter() 抽象材质

不同材质处理入射光线的方式不同。

例如：

1. Lambertian 材质会向表面半球方向进行漫反射散射。
2. Metal 材质根据反射方向和粗糙度生成散射光线。
3. Dielectric 材质需要根据入射角、折射率、全反射和 Schlick 近似，在反射与折射之间进行选择。

如果把这些逻辑全部写进 compute_color()，每增加一种材质都需要修改递归渲染代码，导致材质逻辑和渲染流程耦合。

因此，项目定义 Material 基类，并让不同材质重写 scatter()。compute_color() 只通过统一接口调用：

material->scatter(...)

这样做的好处是：

1. compute_color() 不需要判断具体材质类型。
2. 每种材质的散射逻辑被封装在自己的类中。
3. 增加新材质时，主要新增 Material 派生类，不需要重写递归渲染流程。
4. 几何体只需要保存材质指针，不需要理解材质内部算法。

这体现了面向对象中的多态，也降低了材质模块和渲染流程之间的耦合。

---

### Q5: BVH 在项目中解决了什么问题

如果场景中有 N 个物体，最直接的求交方法是让每条 ray 依次测试所有物体，单条 ray 的求交复杂度接近 O(N)。

BVH 会先使用 AABB 将场景物体组织成树形结构。求交时，ray 先与节点的包围盒进行测试：

1. 如果 ray 没有命中某个节点的 AABB，就可以跳过该节点下的所有物体。
2. 如果 ray 命中 AABB，再继续检查它的子节点或叶子物体。
3. 通过空间剪枝，减少实际执行的精确几何求交次数。

当前项目实现的是场景级 BVH，它管理 Sphere、Plane 或 Mesh 等 Object。一个 Mesh 在场景级 BVH 中仍然被视为一个 Object。

如果 ray 命中 Mesh 的包围盒并进入 Mesh，Mesh 内部目前仍可能线性遍历所有 Triangle。因此，场景级 BVH 能加速不同 Object 之间的查找，但不能完全解决高面数 Mesh 内部的 Triangle 求交瓶颈。

如果以后继续优化，可以为 Mesh 内部单独建立 triangle-level BVH：

Scene BVH → Mesh → Mesh internal BVH → Triangle

场景级 BVH 和 Mesh 内部 BVH 的管理对象不同，不能把二者混为一谈。
## 3. Feature Change Questions

### Q1: 如果要增加一个新材质，你会改哪里

首先新增一个继承 Material 基类的材质类，并重写 scatter() 虚函数，在其中实现该材质的散射规则。

光线与物体相交后，ishit() 会把命中点、法线、UV 和材质指针等信息写入 hit_record。compute_color() 再通过 hit_record 中的材质指针调用：

```cpp
material->scatter(...)
```

增加新材质通常需要：

1. 新增 Material 派生类。
2. 重写 scatter()，根据入射光线和 hit_record 计算 scattered ray 与 attenuation。
3. 在场景构建阶段创建该材质，并绑定到相应几何体。

compute_color() 不需要增加材质类型判断，因为它只依赖 Material 的统一接口。程序运行时会通过虚函数调用具体材质的 scatter()，从而降低渲染流程与具体材质之间的耦合。

---

### Q2: 如果贴图上下颠倒，你会从哪里排查

我会沿着 UV 数据流逐层排查：

```text
OBJ vt → Mesh → Triangle → hit_record → ImageTexture
```

首先检查 OBJLoader 是否正确读取 vt，以及纹理索引是否完成从 1-based 到 0-based 的转换。

然后检查 Mesh 是否把每个顶点对应的 UV 正确传给 Triangle，并检查 Triangle 命中后是否使用重心坐标正确插值 u 和 v。

如果 UV 传递和插值都正确，则重点检查 ImageTexture 中 v 到图片行号的转换。OBJ 的 UV 通常以左下角为原点，而图片像素数组通常从左上角开始，二者的纵轴方向可能相反。

常见处理方式是：

```cpp
v = 1.0 - v;
```

最后可以使用带有明显上下方向标记的测试纹理，判断问题出现在 UV 传递阶段还是图片采样阶段。

---

### Q3: 如果要支持 OBJ 四边形 face，你会怎么修改

不需要新增四边形求交算法，因为当前渲染器以 Triangle 作为基本求交单元。可以在 OBJLoader 解析阶段把四边形拆成两个三角形。

例如四边形角点依次为：

```text
v0, v1, v2, v3
```

可以拆分为：

```text
(v0, v1, v2)
(v0, v2, v3)
```

OBJLoader 不能再假设每个 face 固定只有三个角点，而应该先读取该 face 的全部角点。如果读到四个角点，就生成两个三角形 face。

每个角点中的位置索引、纹理坐标索引和法线索引必须作为整体传递。拆分时还要保持原 face 的顶点绕序，否则可能造成纹理错位或法线翻转。

这种方法适合普通凸四边形。如果需要支持任意凹多边形，则需要更完整的多边形三角化算法。

---

### Q4: 如果要给 Mesh 内部增加 BVH，你会怎么设计

当前场景级 BVH 管理 Sphere、Plane 和 Mesh 等 Object，而 Mesh 内部 BVH 应该管理 Triangle。

设计步骤如下：

1. 让每个 Triangle 通过 getbounding() 返回自己的 AABB。
2. Mesh 完成 triangles 构建后，根据三角形的空间位置递归建立内部 BVH。
3. 构建节点时选择坐标轴，对 Triangle 进行排序和划分。
4. 叶子节点保存一个或少量 Triangle。
5. 非叶子节点保存左右子节点，并使用两个子节点 AABB 的并集作为自身 AABB。
6. Mesh::ishit() 不再线性遍历全部 Triangle，而是调用内部 BVH。
7. 遍历时先测试 ray 与节点 AABB。未命中就跳过整个子树，命中后再访问子节点。
8. 使用 closest_so_far 保存距离 ray 起点最近的命中结果。

为了避免复制大量 Triangle，叶子节点可以保存 Triangle 的指针或索引，但必须保证这些引用在 BVH 使用期间有效。

修改后的路径为：

```text
Scene BVH → Mesh → Mesh internal BVH → Triangle
```

## 4. Debugging Questions

### Q1: 渲染画面全黑怎么排查

我会按照光线从生成到输出的数据流逐层排查：

1. 检查相机是否生成了有效的 primary ray。
2. 暂时输出固定背景色，确认图像写入流程能够工作。
3. 检查场景中是否存在物体，以及物体是否位于相机视野内。
4. 检查场景级 BVH 是否成功构建，根节点 AABB 是否有效。
5. 检查 ray 是否命中物体，必要时绕过 BVH，使用线性求交进行对比。
6. 检查 hit_record 中的 t、p、normal、UV 和 material 是否有效。
7. 检查 material->scatter() 是否返回 true，以及 scattered ray 是否有效。
8. 检查 attenuation 是否过小，导致颜色在递归中被乘成黑色。
9. 检查递归深度是否设置错误，或者是否在第一层就触发终止条件。
10. 检查采样平均、Gamma 校正、clamp 和 PPM 写出阶段。

调试时可以临时返回背景色、法线颜色或 attenuation，从而确定黑色是从哪个阶段开始产生的。

---

### Q2: OBJ 能显示但贴图不显示怎么排查

OBJ 能正常显示，说明顶点位置、face 和 Triangle 求交基本可用。此时应重点排查：

```text
OBJ vt → OBJLoader → Mesh → Triangle → hit_record
→ Material → ImageTexture
```

具体步骤：

1. 检查 OBJ 文件是否包含 vt，以及 face 是否引用了纹理索引。
2. 检查 OBJLoader 是否正确解析 vt。
3. 检查 vt 索引是否从 OBJ 的 1-based 转换为程序的 0-based。
4. 检查 face 中每个顶点是否保存了对应的 vt 索引。
5. 检查 Mesh 是否根据索引取出正确的 uv0、uv1 和 uv2。
6. 检查 Triangle 是否使用重心坐标插值 UV，并写入 hit_record。
7. 检查 Mesh 是否绑定了 ImageTexture，而不是 SolidColorTexture。
8. 检查材质是否使用 rec.u、rec.v 和 rec.p 调用纹理 value()。
9. 检查图片路径、加载结果和文件格式。
10. 使用明显的方向测试纹理，检查 UV 是否始终停留在默认值。

本项目曾出现 OBJ 只显示纯蓝色的问题，原因是 vt 索引没有从 1-based 转换为 0-based，导致 UV 无效并退回默认值。

---

### Q3: 平滑法线没有效果怎么排查

我会沿着顶点法线的数据流排查：

```text
OBJ vn → OBJLoader → OBJData → Mesh → Triangle → hit_record.normal
```

具体步骤：

1. 检查 OBJ 是否包含 vn，以及 face 是否引用了法线索引。
2. 检查 OBJLoader 是否正确解析 vn，并将索引转换为 0-based。
3. 检查 OBJData 是否正确保存 normals 和法线索引。
4. 检查 Mesh 是否根据三个角点的 vn_index 取出 n0、n1 和 n2。
5. 检查 Triangle 命中后，是否使用重心坐标插值三个顶点法线。
6. 检查插值结果是否进行了 normalize。
7. 检查写入 hit_record.normal 的是否为插值法线，而不是平面几何法线。
8. 检查插值法线是否又被后续代码覆盖。
9. 检查最终法线的朝向是否符合程序约定。
10. 如果 OBJ 没有合法 vn，应回退到 Triangle 的几何法线。

调试时可以将法线映射为 RGB 输出。若每个三角形内部颜色完全一致，说明仍在使用平面法线；若颜色连续变化，说明插值法线已经生效。

---

### Q4: 渲染速度突然变慢怎么排查

首先固定测试场景和渲染参数，并记录 Render time、Total rays 和 Rays per second。

排查顺序如下：

1. 检查分辨率是否提高。
2. 检查 samples per pixel 是否增大。
3. 检查最大递归深度是否增大。
4. 检查场景 Object 数量是否增加。
5. 检查 Mesh 的 Triangle 数量是否增加。
6. 检查场景级 BVH 是否成功构建并参与求交。
7. 检查 AABB 是否异常过大，导致剪枝效果下降。
8. 检查程序是否退回线性遍历 Object。
9. 检查 Mesh 内部是否仍然线性遍历全部 Triangle。
10. 检查高频循环中是否加入了 cout、文件写入或调试代码。
11. 检查编译模式和优化参数是否发生变化。
12. 在相同参数下重复测试，排除单次系统波动。

如果 Total rays 增大但 Rays per second 接近原值，通常说明工作量增加；如果 Total rays 接近原值但 Rays per second 明显下降，通常说明单条光线的处理速度变慢。

---

### Q5: GitHub README 没有更新怎么排查

本地修改 README.md 不代表 GitHub 页面已经更新。文件必须经过暂存、提交和推送。

排查顺序如下：

1. 确认终端位于正确的项目目录。
2. 执行 git branch --show-current，确认当前分支。
3. 执行 git status，检查 README.md 的状态。
4. 检查是否执行了 git add README.md。
5. 检查是否执行了 git commit。
6. 使用 git log --oneline -5 确认最新提交是否存在。
7. 检查是否执行了 git push origin main。
8. 阅读 push 输出，确认是否存在网络、认证或远端拒绝问题。
9. 再次执行 git status，确认本地分支是否与 origin/main 同步。
10. 在 GitHub 上确认查看的是正确仓库和分支，然后刷新页面。

如果 README 文字已经更新但图片没有显示，还要检查图片是否被提交、文件名大小写是否一致，以及 Markdown 相对路径是否正确。

## 5. Current Limitations

1. 当前只有场景级 BVH，Mesh 内部仍然线性遍历 Triangle。
2. OBJLoader 主要支持三角形 face，尚未完整支持复杂 OBJ 格式。
3. ImageTexture 当前主要支持 PPM P6。
4. 项目属于学习型渲染器，不是工业级渲染引擎。